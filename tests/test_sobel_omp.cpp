#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <omp.h>

class SobelFilterOMP {
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

public:
    // Aplica el filtro Sobel a una imagen usando OpenMP
    cv::Mat applySobel(const cv::Mat& inputImage) {
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
        
        // Aplicar filtro Sobel con OpenMP
        #pragma omp parallel for collapse(2) schedule(dynamic)
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
    
    // Aplica el filtro Sobel con umbral usando OpenMP
    cv::Mat applySobelWithThreshold(const cv::Mat& inputImage, int threshold = 50) {
        cv::Mat sobelResult = applySobel(inputImage);
        cv::Mat thresholdedImage = cv::Mat::zeros(sobelResult.size(), CV_8UC1);
        
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int i = 0; i < sobelResult.rows; i++) {
            for (int j = 0; j < sobelResult.cols; j++) {
                if (sobelResult.at<uchar>(i, j) > threshold) {
                    thresholdedImage.at<uchar>(i, j) = 255;
                }
            }
        }
        
        return thresholdedImage;
    }
    
    // Versión secuencial para comparación de rendimiento
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

// Función para crear una imagen de prueba
cv::Mat createTestImage() {
    cv::Mat image(400, 400, CV_8UC3, cv::Scalar(128, 128, 128));
    
    // Dibujar algunas formas geométricas
    cv::circle(image, cv::Point(200, 200), 80, cv::Scalar(255, 255, 255), -1);
    cv::rectangle(image, cv::Point(50, 50), cv::Point(150, 150), cv::Scalar(0, 0, 0), -1);
    cv::line(image, cv::Point(300, 100), cv::Point(350, 300), cv::Scalar(255, 0, 0), 5);
    
    // Agregar algo de ruido
    cv::Mat noise(image.size(), image.type());
    cv::randn(noise, cv::Scalar(0, 0, 0), cv::Scalar(20, 20, 20));
    image = image + noise;
    
    return image;
}

int main() {
    // Configurar número de hilos para OpenMP
    int numThreads = omp_get_max_threads();
    omp_set_num_threads(numThreads);
    
    std::cout << "=== Prueba del Filtro Sobel con OpenMP ===" << std::endl;
    std::cout << "Número de hilos disponibles: " << numThreads << std::endl;
    std::cout << "Número de hilos configurados: " << omp_get_num_threads() << std::endl;
    
    std::cout << "Generando imagen de prueba..." << std::endl;
    
    // Crear imagen de prueba
    cv::Mat testImage = createTestImage();
    
    // Guardar imagen de prueba
    cv::imwrite("test_image_omp.jpg", testImage);
    std::cout << "Imagen de prueba guardada como: test_image_omp.jpg" << std::endl;
    
    // Crear instancia del filtro Sobel con OpenMP
    SobelFilterOMP sobelFilter;
    
    // Medir tiempo de ejecución secuencial
    std::cout << "Ejecutando versión secuencial..." << std::endl;
    auto startSeq = std::chrono::high_resolution_clock::now();
    cv::Mat sobelResultSeq = sobelFilter.applySobelSequential(testImage);
    auto endSeq = std::chrono::high_resolution_clock::now();
    auto durationSeq = std::chrono::duration_cast<std::chrono::microseconds>(endSeq - startSeq);
    
    // Medir tiempo de ejecución paralela
    std::cout << "Ejecutando versión paralela con OpenMP..." << std::endl;
    auto startPar = std::chrono::high_resolution_clock::now();
    cv::Mat sobelResultPar = sobelFilter.applySobel(testImage);
    auto endPar = std::chrono::high_resolution_clock::now();
    auto durationPar = std::chrono::duration_cast<std::chrono::microseconds>(endPar - startPar);
    
    // Aplicar filtro con umbral
    std::cout << "Aplicando filtro con umbral..." << std::endl;
    cv::Mat thresholdResult = sobelFilter.applySobelWithThreshold(testImage, 50);
    
    // Guardar resultados
    cv::imwrite("test_sobel_omp_result.jpg", sobelResultPar);
    cv::imwrite("test_sobel_omp_threshold.jpg", thresholdResult);
    
    std::cout << "Resultados guardados:" << std::endl;
    std::cout << "  - test_sobel_omp_result.jpg (filtro Sobel con OpenMP)" << std::endl;
    std::cout << "  - test_sobel_omp_threshold.jpg (filtro con umbral)" << std::endl;
    
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
    cv::compare(sobelResultSeq, sobelResultPar, diff, cv::CMP_NE);
    int differentPixels = cv::countNonZero(diff);
    
    if (differentPixels == 0) {
        std::cout << "✅ Resultados idénticos entre versión secuencial y paralela" << std::endl;
    } else {
        std::cout << "❌ Diferencias encontradas: " << differentPixels << " píxeles" << std::endl;
    }
    
    // Mostrar estadísticas de las imágenes
    std::cout << std::endl;
    std::cout << "=== Estadisticas de las imagenes ===" << std::endl;
    std::cout << "Imagen original: " << testImage.cols << "x" << testImage.rows << " (" << testImage.channels() << " canales)" << std::endl;
    std::cout << "Resultado Sobel: " << sobelResultPar.cols << "x" << sobelResultPar.rows << " (1 canal)" << std::endl;
    
    // Calcular estadísticas del resultado
    double minVal, maxVal;
    cv::minMaxLoc(sobelResultPar, &minVal, &maxVal);
    std::cout << "Valor mínimo en resultado Sobel: " << minVal << std::endl;
    std::cout << "Valor máximo en resultado Sobel: " << maxVal << std::endl;
    
    // Contar píxeles con umbral
    int thresholdedPixels = cv::countNonZero(thresholdResult);
    int totalPixels = thresholdResult.rows * thresholdResult.cols;
    double percentage = (double)thresholdedPixels / totalPixels * 100.0;
    std::cout << "Píxeles con umbral: " << thresholdedPixels << " de " << totalPixels << " (" << percentage << "%)" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Prueba completada exitosamente!" << std::endl;
    return 0;
} 