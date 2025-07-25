#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

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

int main(int argc, char** argv) {
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
    
    // Crear instancia del filtro Sobel
    SobelFilter sobelFilter;
    
    // Aplicar filtro Sobel
    cv::Mat outputImage = sobelFilter.applySobel(inputImage);
    
    // Aplicar filtro con umbral para mejor visualización
    cv::Mat thresholdedImage = sobelFilter.applySobelWithThreshold(inputImage, 50);
    
    // Guardar resultados
    if (cv::imwrite(argv[2], outputImage)) {
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
    cv::namedWindow("Filtro Sobel", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Sobel con Umbral", cv::WINDOW_AUTOSIZE);
    
    cv::imshow("Imagen Original", inputImage);
    cv::imshow("Filtro Sobel", outputImage);
    cv::imshow("Sobel con Umbral", thresholdedImage);
    
    std::cout << "Presiona cualquier tecla para cerrar las ventanas..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();
    
    return 0;
} 