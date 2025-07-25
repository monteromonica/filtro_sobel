#include <jni.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "filter_factory.h"
#include <opencv2/opencv.hpp>

// =============================================================
//  SOBEL_JNI.CPP
//  -----------------------------------------------------------
//  Archivo puente JNI para exponer los filtros de C++ a Android
//  usando el patrón Factory/Strategy. Permite crear, destruir
//  y usar filtros desde Java/Kotlin de forma segura.
// =============================================================

// Mapa para gestionar instancias de filtros en memoria nativa
static std::unordered_map<jlong, std::unique_ptr<EdgeDetectionStrategy>> filters;
static jlong nextId = 1;

extern "C" JNIEXPORT jlong JNICALL
Java_com_photonicsens_sobel_SobelFilter_createFilter(JNIEnv* env, jobject, jstring filterType) {
    const char* typeStr = env->GetStringUTFChars(filterType, nullptr);
    auto filter = FilterFactory::createFilter(typeStr);
    env->ReleaseStringUTFChars(filterType, typeStr);

    if (!filter) return 0;
    jlong id = nextId++;
    filters[id] = std::move(filter);
    return id;
}

extern "C" JNIEXPORT void JNICALL
Java_com_photonicsens_sobel_SobelFilter_destroyFilter(JNIEnv*, jobject, jlong id) {
    filters.erase(id);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_photonicsens_sobel_SobelFilter_processImage(JNIEnv* env, jobject, jlong id, jbyteArray input, jint width, jint height) {
    // Convertir jbyteArray a cv::Mat
    jsize len = env->GetArrayLength(input);
    std::vector<uchar> buffer(len);
    env->GetByteArrayRegion(input, 0, len, reinterpret_cast<jbyte*>(buffer.data()));
    cv::Mat inputImage(height, width, CV_8UC3, buffer.data());

    // Procesar imagen
    auto it = filters.find(id);
    if (it == filters.end()) return nullptr;
    auto resultOpt = it->second->detectEdges(inputImage);
    if (!resultOpt) return nullptr;
    cv::Mat result = *resultOpt;

    // Convertir resultado a jbyteArray
    std::vector<uchar> outBuf;
    cv::imencode(".jpg", result, outBuf);
    jbyteArray output = env->NewByteArray(outBuf.size());
    env->SetByteArrayRegion(output, 0, outBuf.size(), reinterpret_cast<jbyte*>(outBuf.data()));
    return output;
} 