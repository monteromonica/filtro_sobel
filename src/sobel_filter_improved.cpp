// =============================================================
//  SOBEL_FILTER_IMPROVED.CPP
//  -----------------------------------------------------------
//  Esta versión es secuencial profesional, con C++ moderno,
//  manejo robusto de errores y arquitectura preparada para
//  patrones de diseño y Android NDK/JNI.
//  -----------------------------------------------------------
//  No incluye multihilo a propósito para mantener la claridad
//  y la comparación directa con las versiones paralelas.
//  Las versiones multihilo (OpenMP y pThreads) están en archivos
//  separados para facilitar la comparación y la extensibilidad.
// =============================================================

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <optional>
#include <array>

/**
 * @brief Excepción personalizada para errores del filtro Sobel
 */
class SobelFilterException : public std::runtime_error {
public:
    explicit SobelFilterException(const std::string& message) 
        : std::runtime_error("SobelFilter: " + message) {}
};

/**
 * @brief Excepción para imágenes inválidas
 */
class InvalidImageException : public SobelFilterException {
public:
    explicit InvalidImageException(const std::string& details)
        : SobelFilterException("Invalid image: " + details) {}
};

/**
 * @brief Configuración del filtro Sobel
 */
struct FilterConfig {
    int threshold = 80;  // Aumentado de 50 a 80
    bool normalize = true;
    bool useGaussianBlur = false;
    double gaussianSigma = 1.0;
    
    // Validación de configuración
    void validate() const {
        if (threshold < 0 || threshold > 255) {
            throw SobelFilterException("Threshold must be between 0 and 255");
        }
        if (gaussianSigma <= 0) {
            throw SobelFilterException("Gaussian sigma must be positive");
        }
    }
};

/**
 * @brief Filtro de detección de bordes usando el operador de Sobel
 * 
 * Esta clase implementa el algoritmo de Sobel para detectar bordes
 * en imágenes. Utiliza dos kernels 3x3 para calcular gradientes
 * en las direcciones X e Y.
 * 
 * Características:
 * - Implementación manual del algoritmo (no usa OpenCV para el filtro)
 * - Soporte para configuración flexible
 * - Manejo robusto de errores
 * - Optimizaciones de rendimiento
 * 
 * @example
 * SobelFilter filter;
 * auto result = filter.applyFilter(input_image);
 * if (result) {
 *     cv::imwrite("output.jpg", *result);
 * }
 * 
 * @see https://en.wikipedia.org/wiki/Sobel_operator
 */
class SobelFilter {
private:
    // Kernels del filtro Sobel como constantes en tiempo de compilación
    static constexpr std::array<std::array<int, 3>, 3> SOBEL_X = {{
        {{-1, 0, 1}},
        {{-2, 0, 2}},
        {{-1, 0, 1}}
    }};
    
    static constexpr std::array<std::array<int, 3>, 3> SOBEL_Y = {{
        {{-1, -2, -1}},
        {{ 0,  0,  0}},
        {{ 1,  2,  1}}
    }};
    
    static constexpr int KERNEL_SIZE = 3;
    static constexpr int KERNEL_OFFSET = KERNEL_SIZE / 2;
    
    FilterConfig config_;
    std::unique_ptr<cv::Mat> cached_result_;
    
    /**
     * @brief Valida que la imagen de entrada sea válida
     * @param input Imagen de entrada
     * @throws InvalidImageException si la imagen no es válida
     */
    void validateInput(const cv::Mat& input) const {
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
    
    /**
     * @brief Convierte imagen a escala de grises si es necesario
     * @param input Imagen de entrada
     * @return Imagen en escala de grises
     */
    cv::Mat convertToGrayscale(const cv::Mat& input) const {
        cv::Mat grayImage;
        if (input.channels() == 3) {
            cv::cvtColor(input, grayImage, cv::COLOR_BGR2GRAY);
        } else {
            grayImage = input.clone();
        }
        return grayImage;
    }
    
    /**
     * @brief Aplica kernel de convolución a un píxel
     * @param image Imagen de entrada
     * @param row Fila del píxel
     * @param col Columna del píxel
     * @param kernel Kernel a aplicar
     * @return Valor resultante de la convolución
     */
    int applyKernel(const cv::Mat& image, int row, int col, 
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
    
    /**
     * @brief Calcula la magnitud del gradiente
     * @param gx Gradiente en dirección X
     * @param gy Gradiente en dirección Y
     * @return Magnitud del gradiente
     */
    double calculateMagnitude(int gx, int gy) const {
        return std::sqrt(static_cast<double>(gx * gx + gy * gy));
    }
    
    /**
     * @brief Normaliza el valor al rango [0, 255]
     * @param value Valor a normalizar
     * @return Valor normalizado
     */
    uchar normalizeValue(double value) const {
        if (config_.normalize) {
            return static_cast<uchar>(std::min(255.0, std::max(0.0, value)));
        } else {
            return static_cast<uchar>(value);
        }
    }

public:
    /**
     * @brief Constructor con configuración opcional
     * @param config Configuración del filtro
     */
    explicit SobelFilter(const FilterConfig& config = FilterConfig{}) 
        : config_(config) {
        config_.validate();
    }
    
    /**
     * @brief Constructor por movimiento
     */
    SobelFilter(SobelFilter&& other) noexcept = default;
    
    /**
     * @brief Operador de asignación por movimiento
     */
    SobelFilter& operator=(SobelFilter&& other) noexcept = default;
    
    /**
     * @brief Destructor virtual
     */
    virtual ~SobelFilter() = default;
    
    /**
     * @brief Aplica el filtro Sobel a una imagen
     * @param input Imagen de entrada
     * @return Imagen procesada o std::nullopt si hay error
     */
    std::optional<cv::Mat> applyFilter(const cv::Mat& input) const {
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
    
    /**
     * @brief Aplica el filtro Sobel con umbral
     * @param input Imagen de entrada
     * @param threshold Umbral (usa el configurado si es -1)
     * @return Imagen procesada con umbral o std::nullopt si hay error
     */
    std::optional<cv::Mat> applyFilterWithThreshold(const cv::Mat& input, int threshold = -1) const {
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
    
    // Getters y setters para configuración
    void setThreshold(int threshold) { 
        config_.threshold = threshold; 
        config_.validate();
    }
    
    int getThreshold() const { return config_.threshold; }
    
    void setNormalize(bool normalize) { config_.normalize = normalize; }
    
    bool getNormalize() const { return config_.normalize; }
    
    void setUseGaussianBlur(bool useBlur) { config_.useGaussianBlur = useBlur; }
    
    bool getUseGaussianBlur() const { return config_.useGaussianBlur; }
    
    void setGaussianSigma(double sigma) { 
        config_.gaussianSigma = sigma; 
        config_.validate();
    }
    
    double getGaussianSigma() const { return config_.gaussianSigma; }
    
    /**
     * @brief Obtiene información sobre el filtro
     * @return String con información del filtro
     */
    std::string getInfo() const {
        return "SobelFilter[threshold=" + std::to_string(config_.threshold) + 
               ", normalize=" + std::to_string(config_.normalize) + 
               ", gaussianBlur=" + std::to_string(config_.useGaussianBlur) + 
               ", sigma=" + std::to_string(config_.gaussianSigma) + "]";
    }
};

int main(int argc, char** argv) {
    try {
        // Verificar argumentos de línea de comandos
        if (argc != 3) {
            std::cout << "Uso: " << argv[0] << " <imagen_entrada> <imagen_salida>" << std::endl;
            std::cout << "Ejemplo: " << argv[0] << " input.jpg output.jpg" << std::endl;
            return -1;
        }
        
        // Cargar imagen de entrada
        cv::Mat inputImage = cv::imread(argv[1]);
        if (inputImage.empty()) {
            throw InvalidImageException("No se pudo cargar la imagen " + std::string(argv[1]));
        }
        
        std::cout << "Imagen cargada: " << inputImage.cols << "x" << inputImage.rows 
                  << " (" << inputImage.channels() << " canales)" << std::endl;
        
        // Crear configuración del filtro
        FilterConfig config;
        config.threshold = 50;
        config.normalize = true;
        config.useGaussianBlur = false;
        
        // Crear instancia del filtro Sobel
        SobelFilter sobelFilter(config);
        std::cout << "Filtro configurado: " << sobelFilter.getInfo() << std::endl;
        
        // Aplicar filtro Sobel
        auto outputImage = sobelFilter.applyFilter(inputImage);
        if (!outputImage) {
            throw SobelFilterException("Error al aplicar el filtro Sobel");
        }
        
        // Aplicar filtro con umbral
        auto thresholdedImage = sobelFilter.applyFilterWithThreshold(inputImage);
        if (!thresholdedImage) {
            throw SobelFilterException("Error al aplicar el filtro con umbral");
        }
        
        // Guardar resultados
        if (!cv::imwrite(argv[2], *outputImage)) {
            throw SobelFilterException("No se pudo guardar la imagen de salida");
        }
        std::cout << "Imagen procesada guardada como: " << argv[2] << std::endl;
        
        // Guardar versión con umbral
        std::string thresholdFilename = std::string(argv[2]);
        size_t dotPos = thresholdFilename.find_last_of('.');
        if (dotPos != std::string::npos) {
            thresholdFilename = thresholdFilename.substr(0, dotPos) + "_threshold" + thresholdFilename.substr(dotPos);
        } else {
            thresholdFilename += "_threshold";
        }
        
        if (!cv::imwrite(thresholdFilename, *thresholdedImage)) {
            throw SobelFilterException("No se pudo guardar la imagen con umbral");
        }
        std::cout << "Imagen con umbral guardada como: " << thresholdFilename << std::endl;
        
        // Mostrar imágenes (opcional)
        cv::namedWindow("Imagen Original", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Filtro Sobel", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Sobel con Umbral", cv::WINDOW_AUTOSIZE);
        
        cv::imshow("Imagen Original", inputImage);
        cv::imshow("Filtro Sobel", *outputImage);
        cv::imshow("Sobel con Umbral", *thresholdedImage);
        
        std::cout << "Presiona cualquier tecla para cerrar las ventanas..." << std::endl;
        cv::waitKey(0);
        cv::destroyAllWindows();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
} 