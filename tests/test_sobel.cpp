#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

// Incluir la clase SobelFilter del archivo principal
class SobelFilter {
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
    // Aplica el filtro Sobel a una imagen
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
    
    // Aplica el filtro Sobel con umbral para obtener bordes más definidos
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

// Función para crear una imagen de prueba con formas geométricas
cv::Mat createTestImage(int width = 400, int height = 400) {
    cv::Mat testImage = cv::Mat::zeros(height, width, CV_8UC3);
    
    // Dibujar un rectángulo
    cv::rectangle(testImage, cv::Point(50, 50), cv::Point(150, 150), cv::Scalar(255, 255, 255), -1);
    
    // Dibujar un círculo
    cv::circle(testImage, cv::Point(300, 100), 50, cv::Scalar(255, 255, 255), -1);
    
    // Dibujar líneas
    cv::line(testImage, cv::Point(50, 250), cv::Point(350, 250), cv::Scalar(255, 255, 255), 3);
    cv::line(testImage, cv::Point(200, 200), cv::Point(200, 350), cv::Scalar(255, 255, 255), 3);
    
    // Dibujar un triángulo
    std::vector<cv::Point> triangle = {
        cv::Point(100, 300),
        cv::Point(150, 350),
        cv::Point(200, 300)
    };
    cv::fillPoly(testImage, std::vector<std::vector<cv::Point>>{triangle}, cv::Scalar(255, 255, 255));
    
    return testImage;
}

int main() {
    std::cout << "=== Prueba del Filtro Sobel ===" << std::endl;
    std::cout << "Generando imagen de prueba..." << std::endl;
    
    // Crear imagen de prueba
    cv::Mat testImage = createTestImage();
    
    // Guardar imagen de prueba
    cv::imwrite("test_image.jpg", testImage);
    std::cout << "Imagen de prueba guardada como: test_image.jpg" << std::endl;
    
    // Crear instancia del filtro Sobel
    SobelFilter sobelFilter;
    
    // Aplicar filtro Sobel
    std::cout << "Aplicando filtro Sobel..." << std::endl;
    cv::Mat sobelResult = sobelFilter.applySobel(testImage);
    
    // Aplicar filtro con umbral
    std::cout << "Aplicando filtro con umbral..." << std::endl;
    cv::Mat thresholdResult = sobelFilter.applySobelWithThreshold(testImage, 50);
    
    // Guardar resultados
    cv::imwrite("test_sobel_result.jpg", sobelResult);
    cv::imwrite("test_sobel_threshold.jpg", thresholdResult);
    
    std::cout << "Resultados guardados:" << std::endl;
    std::cout << "  - test_sobel_result.jpg (filtro Sobel)" << std::endl;
    std::cout << "  - test_sobel_threshold.jpg (filtro con umbral)" << std::endl;
    
    // Mostrar imágenes
    cv::namedWindow("Imagen de Prueba", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Filtro Sobel", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Sobel con Umbral", cv::WINDOW_AUTOSIZE);
    
    cv::imshow("Imagen de Prueba", testImage);
    cv::imshow("Filtro Sobel", sobelResult);
    cv::imshow("Sobel con Umbral", thresholdResult);
    
    std::cout << std::endl;
    std::cout << "Imágenes mostradas. Presiona cualquier tecla para cerrar..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();
    
    std::cout << "Prueba completada exitosamente!" << std::endl;
    return 0;
} 