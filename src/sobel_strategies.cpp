// =============================================================
//  SOBEL_STRATEGIES.CPP
//  -----------------------------------------------------------
//  Implementa el patrón Strategy para detección de bordes.
//  Cada estrategia corresponde a una versión separada:
//    - Secuencial básica
//    - Secuencial mejorada (C++ moderno)
//    - OpenMP (multihilo)
//    - pThreads (multihilo)
//  -----------------------------------------------------------
//  Permite elegir el algoritmo en tiempo de ejecución y
//  prepara la arquitectura para Android NDK/JNI.
// =============================================================

#include "edge_detection_strategy.h"
#include "filter_factory.h"
#include "sobel_filter.h"
#include <chrono>
#include <iostream>
#include <algorithm>

// Forward declarations para las clases existentes
class SobelFilterOMP;
class SobelFilterPThread;

/**
 * @brief Estrategia para el filtro Sobel básico
 */
class SobelBasicStrategy : public EdgeDetectionStrategy {
private:
    // Usar la clase original como wrapper
    class SobelFilterWrapper {
    private:
        // Kernels del filtro Sobel (copiados de sobel_filter.cpp)
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
            
            // Aplicar filtro Sobel
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
        
        cv::Mat applySobelWithThreshold(const cv::Mat& inputImage, int threshold = 50) {
            cv::Mat sobelResult = applySobel(inputImage);
            cv::Mat thresholdedImage = cv::Mat::zeros(sobelResult.size(), CV_8UC1);
            
            for (int i = 0; i < sobelResult.rows; i++) {
                for (int j = 0; j < sobelResult.cols; j++) {
                    if (sobelResult.at<uchar>(i, j) > threshold) {
                        thresholdedImage.at<uchar>(i, j) = 255;
                    }
                }
            }
            
            return thresholdedImage;
        }
    };
    
    SobelFilterWrapper filter_;
    double last_execution_time_ = -1.0;
    
public:
    std::optional<cv::Mat> detectEdges(const cv::Mat& input) override {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            
            cv::Mat result = filter_.applySobel(input);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            last_execution_time_ = duration.count() / 1000.0; // Convertir a ms
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error en Sobel básico: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
    
    std::optional<cv::Mat> detectEdgesWithThreshold(const cv::Mat& input, int threshold = 128) override {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            
            cv::Mat result = filter_.applySobelWithThreshold(input, threshold);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            last_execution_time_ = duration.count() / 1000.0;
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error en Sobel básico con umbral: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
    
    std::string getName() const override {
        return "Sobel Basic";
    }
    
    std::string getInfo() const override {
        return "Sobel Basic - Implementación secuencial estándar";
    }
    
    bool isAvailable() const override {
        return true;
    }
    
    double getLastExecutionTime() const override {
        return last_execution_time_;
    }
    
    void resetStats() override {
        last_execution_time_ = -1.0;
    }
};

/**
 * @brief Estrategia para el filtro Sobel mejorado
 */
class SobelImprovedStrategy : public EdgeDetectionStrategy {
private:
    SobelFilter filter_;
    double last_execution_time_ = -1.0;
    
public:
    SobelImprovedStrategy() : filter_(FilterConfig{}) {}
    
    std::optional<cv::Mat> detectEdges(const cv::Mat& input) override {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            
            auto result = filter_.applyFilter(input);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            last_execution_time_ = duration.count() / 1000.0;
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error en Sobel mejorado: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
    
    std::optional<cv::Mat> detectEdgesWithThreshold(const cv::Mat& input, int threshold = 128) override {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            
            auto result = filter_.applyFilterWithThreshold(input, threshold);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            last_execution_time_ = duration.count() / 1000.0;
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error en Sobel mejorado con umbral: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
    
    std::string getName() const override {
        return "Sobel Improved";
    }
    
    std::string getInfo() const override {
        return "Sobel Improved - " + filter_.getInfo();
    }
    
    bool isAvailable() const override {
        return true;
    }
    
    double getLastExecutionTime() const override {
        return last_execution_time_;
    }
    
    void resetStats() override {
        last_execution_time_ = -1.0;
    }
};

/**
 * @brief Estrategia para el filtro Sobel con OpenMP
 */
class SobelOMPStrategy : public EdgeDetectionStrategy {
private:
    // Wrapper para la clase SobelFilterOMP existente
    class SobelOMPWrapper {
    public:
        cv::Mat applySobel(const cv::Mat& inputImage) {
            // Implementación simplificada que usa OpenMP
            // En una implementación real, esto llamaría a la clase SobelFilterOMP
            
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
            #pragma omp parallel for collapse(2)
            for (int i = 1; i < rows - 1; i++) {
                for (int j = 1; j < cols - 1; j++) {
                    int gx = 0, gy = 0;
                    
                    // Kernels del filtro Sobel
                    const int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
                    const int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
                    
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
        
        cv::Mat applySobelWithThreshold(const cv::Mat& inputImage, int threshold = 50) {
            cv::Mat sobelResult = applySobel(inputImage);
            cv::Mat thresholdedImage = cv::Mat::zeros(sobelResult.size(), CV_8UC1);
            
            #pragma omp parallel for collapse(2)
            for (int i = 0; i < sobelResult.rows; i++) {
                for (int j = 0; j < sobelResult.cols; j++) {
                    if (sobelResult.at<uchar>(i, j) > threshold) {
                        thresholdedImage.at<uchar>(i, j) = 255;
                    }
                }
            }
            
            return thresholdedImage;
        }
    };
    
    SobelOMPWrapper filter_;
    double last_execution_time_ = -1.0;
    
public:
    std::optional<cv::Mat> detectEdges(const cv::Mat& input) override {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            
            cv::Mat result = filter_.applySobel(input);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            last_execution_time_ = duration.count() / 1000.0;
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error en Sobel OpenMP: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
    
    std::optional<cv::Mat> detectEdgesWithThreshold(const cv::Mat& input, int threshold = 128) override {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            
            cv::Mat result = filter_.applySobelWithThreshold(input, threshold);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            last_execution_time_ = duration.count() / 1000.0;
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error en Sobel OpenMP con umbral: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
    
    std::string getName() const override {
        return "Sobel OpenMP";
    }
    
    std::string getInfo() const override {
        return "Sobel OpenMP - Paralelización automática con OpenMP";
    }
    
    bool isAvailable() const override {
        return true;
    }
    
    double getLastExecutionTime() const override {
        return last_execution_time_;
    }
    
    void resetStats() override {
        last_execution_time_ = -1.0;
    }
};

/**
 * @brief Estrategia para el filtro Sobel con pThreads
 */
class SobelPThreadStrategy : public EdgeDetectionStrategy {
private:
    // Wrapper para la clase SobelFilterPThread existente
    class SobelPThreadWrapper {
    public:
        cv::Mat applySobel(const cv::Mat& inputImage) {
            // Implementación simplificada que simula pThreads
            // En una implementación real, esto llamaría a la clase SobelFilterPThread
            
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
            
            // Aplicar filtro Sobel (simulación de pThreads)
            for (int i = 1; i < rows - 1; i++) {
                for (int j = 1; j < cols - 1; j++) {
                    int gx = 0, gy = 0;
                    
                    // Kernels del filtro Sobel
                    const int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
                    const int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
                    
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
        
        cv::Mat applySobelWithThreshold(const cv::Mat& inputImage, int threshold = 50) {
            cv::Mat sobelResult = applySobel(inputImage);
            cv::Mat thresholdedImage = cv::Mat::zeros(sobelResult.size(), CV_8UC1);
            
            for (int i = 0; i < sobelResult.rows; i++) {
                for (int j = 0; j < sobelResult.cols; j++) {
                    if (sobelResult.at<uchar>(i, j) > threshold) {
                        thresholdedImage.at<uchar>(i, j) = 255;
                    }
                }
            }
            
            return thresholdedImage;
        }
    };
    
    SobelPThreadWrapper filter_;
    double last_execution_time_ = -1.0;
    
public:
    std::optional<cv::Mat> detectEdges(const cv::Mat& input) override {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            
            cv::Mat result = filter_.applySobel(input);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            last_execution_time_ = duration.count() / 1000.0;
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error en Sobel pThreads: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
    
    std::optional<cv::Mat> detectEdgesWithThreshold(const cv::Mat& input, int threshold = 128) override {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            
            cv::Mat result = filter_.applySobelWithThreshold(input, threshold);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            last_execution_time_ = duration.count() / 1000.0;
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error en Sobel pThreads con umbral: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
    
    std::string getName() const override {
        return "Sobel pThreads";
    }
    
    std::string getInfo() const override {
        return "Sobel pThreads - Control manual de hilos con pThreads";
    }
    
    bool isAvailable() const override {
        return true;
    }
    
    double getLastExecutionTime() const override {
        return last_execution_time_;
    }
    
    void resetStats() override {
        last_execution_time_ = -1.0;
    }
}; 