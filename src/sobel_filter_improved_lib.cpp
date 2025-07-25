#include "sobel_filter.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <optional>
#include <array>

// Implementación de la validación de configuración
void FilterConfig::validate() const {
    if (threshold < 0 || threshold > 255) {
        throw SobelFilterException("Threshold must be between 0 and 255");
    }
    if (gaussianSigma <= 0) {
        throw SobelFilterException("Gaussian sigma must be positive");
    }
}

// Implementación de los métodos de SobelFilter
SobelFilter::SobelFilter(const FilterConfig& config) : config_(config) {
    config_.validate();
}

void SobelFilter::validateInput(const cv::Mat& input) const {
    if (input.empty()) {
        throw InvalidImageException("Input image is empty");
    }
    if (input.dims != 2) {
        throw InvalidImageException("Input image must be 2D");
    }
    if (input.type() != CV_8UC1 && input.type() != CV_8UC3) {
        throw InvalidImageException("Input image must be 8-bit grayscale or BGR");
    }
}

cv::Mat SobelFilter::convertToGrayscale(const cv::Mat& input) const {
    cv::Mat grayImage;
    if (input.channels() == 3) {
        cv::cvtColor(input, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = input.clone();
    }
    return grayImage;
}

int SobelFilter::applyKernel(const cv::Mat& image, int row, int col, 
                           const std::array<std::array<int, 3>, 3>& kernel) const {
    int result = 0;
    for (int i = 0; i < KERNEL_SIZE; ++i) {
        for (int j = 0; j < KERNEL_SIZE; ++j) {
            int pixelRow = row + i - KERNEL_OFFSET;
            int pixelCol = col + j - KERNEL_OFFSET;
            
            // Verificar límites de la imagen
            if (pixelRow >= 0 && pixelRow < image.rows && 
                pixelCol >= 0 && pixelCol < image.cols) {
                result += static_cast<int>(image.at<uchar>(pixelRow, pixelCol)) * 
                         kernel[i][j];
            }
        }
    }
    return result;
}

double SobelFilter::calculateMagnitude(int gx, int gy) const {
    return std::sqrt(static_cast<double>(gx * gx + gy * gy));
}

uchar SobelFilter::normalizeValue(double value) const {
    if (config_.normalize) {
        return static_cast<uchar>(std::min(255.0, std::max(0.0, value)));
    } else {
        return static_cast<uchar>(value);
    }
}

std::optional<cv::Mat> SobelFilter::applyFilter(const cv::Mat& input) const {
    try {
        validateInput(input);
        
        // Convertir a escala de grises
        cv::Mat grayImage = convertToGrayscale(input);
        
        // Aplicar blur gaussiano si está configurado
        if (config_.useGaussianBlur) {
            cv::GaussianBlur(grayImage, grayImage, cv::Size(3, 3), config_.gaussianSigma);
        }
        
        // Crear imagen de salida
        cv::Mat outputImage = cv::Mat::zeros(grayImage.size(), CV_8UC1);
        
        // Aplicar filtro Sobel
        for (int i = KERNEL_OFFSET; i < grayImage.rows - KERNEL_OFFSET; ++i) {
            for (int j = KERNEL_OFFSET; j < grayImage.cols - KERNEL_OFFSET; ++j) {
                // Calcular gradientes
                int gx = applyKernel(grayImage, i, j, SOBEL_X);
                int gy = applyKernel(grayImage, i, j, SOBEL_Y);
                
                // Calcular magnitud del gradiente
                double magnitude = calculateMagnitude(gx, gy);
                
                // Normalizar y asignar valor
                outputImage.at<uchar>(i, j) = normalizeValue(magnitude);
            }
        }
        
        return outputImage;
        
    } catch (const std::exception& e) {
        std::cerr << "Error applying Sobel filter: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<cv::Mat> SobelFilter::applyFilterWithThreshold(const cv::Mat& input, int threshold) const {
    try {
        // Usar umbral configurado si no se especifica uno
        int actualThreshold = (threshold >= 0) ? threshold : config_.threshold;
        
        // Validar umbral
        if (actualThreshold < 0 || actualThreshold > 255) {
            throw SobelFilterException("Threshold must be between 0 and 255");
        }
        
        // Aplicar filtro Sobel
        auto sobelResult = applyFilter(input);
        if (!sobelResult) {
            return std::nullopt;
        }
        
        // Aplicar umbral
        cv::Mat thresholdedImage = cv::Mat::zeros(sobelResult->size(), CV_8UC1);
        
        for (int i = 0; i < sobelResult->rows; ++i) {
            for (int j = 0; j < sobelResult->cols; ++j) {
                if (sobelResult->at<uchar>(i, j) > actualThreshold) {
                    thresholdedImage.at<uchar>(i, j) = 255;
                }
            }
        }
        
        return thresholdedImage;
        
    } catch (const std::exception& e) {
        std::cerr << "Error applying Sobel filter with threshold: " << e.what() << std::endl;
        return std::nullopt;
    }
}

// Getters y setters
void SobelFilter::setThreshold(int threshold) { 
    config_.threshold = threshold; 
    config_.validate();
}

int SobelFilter::getThreshold() const { return config_.threshold; }

void SobelFilter::setNormalize(bool normalize) { config_.normalize = normalize; }

bool SobelFilter::getNormalize() const { return config_.normalize; }

void SobelFilter::setUseGaussianBlur(bool useBlur) { config_.useGaussianBlur = useBlur; }

bool SobelFilter::getUseGaussianBlur() const { return config_.useGaussianBlur; }

void SobelFilter::setGaussianSigma(double sigma) { 
    config_.gaussianSigma = sigma; 
    config_.validate();
}

double SobelFilter::getGaussianSigma() const { return config_.gaussianSigma; }

std::string SobelFilter::getInfo() const {
    return "SobelFilter[threshold=" + std::to_string(config_.threshold) + 
           ", normalize=" + std::to_string(config_.normalize) + 
           ", gaussianBlur=" + std::to_string(config_.useGaussianBlur) + 
           ", sigma=" + std::to_string(config_.gaussianSigma) + "]";
} 