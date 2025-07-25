// =============================================================
//  SOBEL_FILTER_PTHREAD.CPP
//  -----------------------------------------------------------
//  Esta versión implementa el filtro Sobel usando pThreads
//  para paralelización manual. Optimizada solo para
//  rendimiento y comparación con la versión secuencial.
//  -----------------------------------------------------------
//  Mantiene la lógica separada para facilitar la comparación
//  y la extensibilidad. No incluye mejoras de C++ moderno
//  para mantener el foco en el multihilo.
// =============================================================

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <pthread.h>
#include <thread>

class SobelFilterPThread {
private:
    // Kernels del filtro Sobel
    const std::vector<std::vector<int>> sobelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    
    const std::vector<std::vector<int>> sobelY = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    // Estructura para pasar datos a los hilos
    struct ThreadData {
        const cv::Mat* inputImage;
        cv::Mat* outputImage;
        int startRow;
        int endRow;
        int startCol;
        int endCol;
        const std::vector<std::vector<int>>* sobelX;
        const std::vector<std::vector<int>>* sobelY;
    };

    // Función que ejecuta cada hilo
    static void* sobelThread(void* arg) {
        ThreadData* data = static_cast<ThreadData*>(arg);
        
        // Convertir a escala de grises si es necesario
        cv::Mat grayImage;
        if (data->inputImage->channels() == 3) {
            cv::cvtColor(*data->inputImage, grayImage, cv::COLOR_BGR2GRAY);
        } else {
            grayImage = data->inputImage->clone();
        }
        
        // Aplicar filtro Sobel en la región asignada
        for (int i = data->startRow; i < data->endRow; i++) {
            for (int j = data->startCol; j < data->endCol; j++) {
                if (i > 0 && i < grayImage.rows - 1 && j > 0 && j < grayImage.cols - 1) {
                    int gx = 0, gy = 0;
                    
                    // Calcular gradientes usando los kernels
                    for (int ki = -1; ki <= 1; ki++) {
                        for (int kj = -1; kj <= 1; kj++) {
                            int pixelValue = static_cast<int>(grayImage.at<uchar>(i + ki, j + kj));
                            gx += pixelValue * (*data->sobelX)[ki + 1][kj + 1];
                            gy += pixelValue * (*data->sobelY)[ki + 1][kj + 1];
                        }
                    }
                    
                    // Calcular magnitud del gradiente
                    double magnitude = std::sqrt(gx * gx + gy * gy);
                    
                    // Normalizar y asignar valor
                    magnitude = std::min(255.0, magnitude);
                    data->outputImage->at<uchar>(i, j) = static_cast<uchar>(magnitude);
                }
            }
        }
        
        return nullptr;
    }

public:
    // Aplica el filtro Sobel usando pThreads
    cv::Mat applySobel(const cv::Mat& inputImage, int numThreads = 4) {
        // Convertir a escala de grises si es necesario
        cv::Mat grayImage;
        if (inputImage.channels() == 3) {
            cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        } else {
            grayImage = inputImage.clone();
        }
        
        // Crear imagen de salida
        cv::Mat outputImage = cv::Mat::zeros(grayImage.size(), CV_8UC1);
        
        int rows = grayImage.rows;
        int cols = grayImage.cols;
        
        // Crear hilos
        std::vector<pthread_t> threads(numThreads);
        std::vector<ThreadData> threadData(numThreads);
        
        // Dividir el trabajo entre hilos
        int rowsPerThread = rows / numThreads;
        
        for (int t = 0; t < numThreads; t++) {
            threadData[t].inputImage = &grayImage;
            threadData[t].outputImage = &outputImage;
            threadData[t].startRow = t * rowsPerThread;
            threadData[t].endRow = (t == numThreads - 1) ? rows : (t + 1) * rowsPerThread;
            threadData[t].startCol = 0;
            threadData[t].endCol = cols;
            threadData[t].sobelX = &sobelX;
            threadData[t].sobelY = &sobelY;
            
            // Crear hilo
            pthread_create(&threads[t], nullptr, sobelThread, &threadData[t]);
        }
        
        // Esperar a que todos los hilos terminen
        for (int t = 0; t < numThreads; t++) {
            pthread_join(threads[t], nullptr);
        }
        
        return outputImage;
    }
    
    // Aplica el filtro Sobel con umbral usando pThreads
    cv::Mat applySobelWithThreshold(const cv::Mat& inputImage, int threshold = 50, int numThreads = 4) {
        cv::Mat sobelResult = applySobel(inputImage, numThreads);
        cv::Mat thresholdedImage = cv::Mat::zeros(sobelResult.size(), CV_8UC1);
        
        // Aplicar umbral en paralelo usando std::thread (más simple para esta operación)
        std::vector<std::thread> threads;
        int rowsPerThread = sobelResult.rows / numThreads;
        
        for (int t = 0; t < numThreads; t++) {
            int startRow = t * rowsPerThread;
            int endRow = (t == numThreads - 1) ? sobelResult.rows : (t + 1) * rowsPerThread;
            
            threads.emplace_back([&sobelResult, &thresholdedImage, startRow, endRow, threshold]() {
                for (int i = startRow; i < endRow; i++) {
                    for (int j = 0; j < sobelResult.cols; j++) {
                        if (sobelResult.at<uchar>(i, j) > threshold) {
                            thresholdedImage.at<uchar>(i, j) = 255;
                        }
                    }
                }
            });
        }
        
        // Esperar a que todos los hilos terminen
        for (auto& thread : threads) {
            thread.join();
        }
        
        return thresholdedImage;
    }
    
    // Versión secuencial para comparación
    cv::Mat applySobelSequential(const cv::Mat& inputImage) {
        // Convertir a escala de grises si es necesario
        cv::Mat grayImage;
        if (inputImage.channels() == 3) {
            cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        } else {
            grayImage = inputImage.clone();
        }
        
        // Crear imagen de salida
        cv::Mat outputImage = cv::Mat::zeros(grayImage.size(), CV_8UC1);
        
        int rows = grayImage.rows;
        int cols = grayImage.cols;
        
        // Aplicar filtro Sobel secuencial
        for (int i = 1; i < rows - 1; i++) {
            for (int j = 1; j < cols - 1; j++) {
                int gx = 0, gy = 0;
                
                // Calcular gradientes usando los kernels
                for (int ki = -1; ki <= 1; ki++) {
                    for (int kj = -1; kj <= 1; kj++) {
                        int pixelValue = static_cast<int>(grayImage.at<uchar>(i + ki, j + kj));
                        gx += pixelValue * sobelX[ki + 1][kj + 1];
                        gy += pixelValue * sobelY[ki + 1][kj + 1];
                    }
                }
                
                // Calcular magnitud del gradiente
                double magnitude = std::sqrt(gx * gx + gy * gy);
                
                // Normalizar y asignar valor
                magnitude = std::min(255.0, magnitude);
                outputImage.at<uchar>(i, j) = static_cast<uchar>(magnitude);
            }
        }
        
        return outputImage;
    }
};

int main(int argc, char** argv) {
    std::cout << "=== Filtro Sobel con pThreads ===" << std::endl;
    
    // Obtener número de hilos disponibles
    int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4; // Valor por defecto
    
    std::cout << "Número de hilos disponibles: " << numThreads << std::endl;
    
    // Verificar argumentos de línea de comandos
    if (argc != 3) {
        std::cout << "Uso: " << argv[0] << " <imagen_entrada> <imagen_salida>" << std::endl;
        std::cout << "Ejemplo: " << argv[0] << " input.jpg output.jpg" << std::endl;
        return -1;
    }
    
    // Cargar imagen de entrada
    cv::Mat inputImage = cv::imread(argv[1]);
    if (inputImage.empty()) {
        std::cerr << "Error: No se pudo cargar la imagen " << argv[1] << std::endl;
        return -1;
    }
    
    std::cout << "Imagen cargada: " << inputImage.cols << "x" << inputImage.rows 
              << " (" << inputImage.channels() << " canales)" << std::endl;
    
    // Crear instancia del filtro Sobel con pThreads
    SobelFilterPThread sobelFilter;
    
    // Medir tiempo de ejecución secuencial
    auto startSeq = std::chrono::high_resolution_clock::now();
    cv::Mat outputImageSeq = sobelFilter.applySobelSequential(inputImage);
    auto endSeq = std::chrono::high_resolution_clock::now();
    auto durationSeq = std::chrono::duration_cast<std::chrono::microseconds>(endSeq - startSeq);
    
    // Medir tiempo de ejecución paralela
    auto startPar = std::chrono::high_resolution_clock::now();
    cv::Mat outputImagePar = sobelFilter.applySobel(inputImage, numThreads);
    auto endPar = std::chrono::high_resolution_clock::now();
    auto durationPar = std::chrono::duration_cast<std::chrono::microseconds>(endPar - startPar);
    
    // Mostrar resultados de rendimiento
    std::cout << std::endl;
    std::cout << "=== Resultados de Rendimiento ===" << std::endl;
    std::cout << "Tiempo secuencial: " << durationSeq.count() << " microsegundos" << std::endl;
    std::cout << "Tiempo paralelo:   " << durationPar.count() << " microsegundos" << std::endl;
    
    double speedup = static_cast<double>(durationSeq.count()) / durationPar.count();
    std::cout << "Speedup: " << speedup << "x" << std::endl;
    std::cout << "Eficiencia: " << (speedup / numThreads) * 100 << "%" << std::endl;
    
    // Verificar que los resultados son idénticos
    cv::Mat diff;
    cv::compare(outputImageSeq, outputImagePar, diff, cv::CMP_NE);
    int differentPixels = cv::countNonZero(diff);
    
    if (differentPixels == 0) {
        std::cout << "✅ Resultados idénticos entre versión secuencial y paralela" << std::endl;
    } else {
        std::cout << "❌ Diferencias encontradas: " << differentPixels << " píxeles" << std::endl;
    }
    
    // Aplicar filtro con umbral para mejor visualización
    cv::Mat thresholdedImage = sobelFilter.applySobelWithThreshold(inputImage, 50, numThreads);
    
    // Guardar resultados
    if (cv::imwrite(argv[2], outputImagePar)) {
        std::cout << "Imagen procesada guardada como: " << argv[2] << std::endl;
    } else {
        std::cerr << "Error: No se pudo guardar la imagen de salida" << std::endl;
        return -1;
    }
    
    // Guardar versión con umbral
    std::string thresholdFilename = std::string(argv[2]);
    size_t dotPos = thresholdFilename.find_last_of('.');
    if (dotPos != std::string::npos) {
        thresholdFilename = thresholdFilename.substr(0, dotPos) + "_threshold" + thresholdFilename.substr(dotPos);
    } else {
        thresholdFilename += "_threshold";
    }
    
    if (cv::imwrite(thresholdFilename, thresholdedImage)) {
        std::cout << "Imagen con umbral guardada como: " << thresholdFilename << std::endl;
    }
    
    // Mostrar imágenes (opcional)
    cv::namedWindow("Imagen Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Filtro Sobel (pThreads)", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Sobel con Umbral", cv::WINDOW_AUTOSIZE);
    
    cv::imshow("Imagen Original", inputImage);
    cv::imshow("Filtro Sobel (pThreads)", outputImagePar);
    cv::imshow("Sobel con Umbral", thresholdedImage);
    
    std::cout << "Presiona cualquier tecla para cerrar las ventanas..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();
    
    return 0;
} 