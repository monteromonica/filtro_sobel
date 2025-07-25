#ifndef SOBEL_FILTER_H
#define SOBEL_FILTER_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <optional>
#include <array>
#include <type_traits>
#include <string>

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
    int threshold = 50;
    bool normalize = true;
    bool useGaussianBlur = false;
    double gaussianSigma = 1.0;
    
    // Validación de configuración
    void validate() const;
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
    
    // Métodos privados
    void validateInput(const cv::Mat& input) const;
    cv::Mat convertToGrayscale(const cv::Mat& input) const;
    int applyKernel(const cv::Mat& image, int row, int col, 
                   const std::array<std::array<int, 3>, 3>& kernel) const;
    double calculateMagnitude(int gx, int gy) const;
    uchar normalizeValue(double value) const;

public:
    /**
     * @brief Constructor con configuración opcional
     * @param config Configuración del filtro
     */
    explicit SobelFilter(const FilterConfig& config = FilterConfig{});
    
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
    std::optional<cv::Mat> applyFilter(const cv::Mat& input) const;
    
    /**
     * @brief Aplica el filtro Sobel con umbral
     * @param input Imagen de entrada
     * @param threshold Umbral (usa el configurado si es -1)
     * @return Imagen procesada con umbral o std::nullopt si hay error
     */
    std::optional<cv::Mat> applyFilterWithThreshold(const cv::Mat& input, int threshold = -1) const;
    
    // Getters y setters para configuración
    void setThreshold(int threshold);
    int getThreshold() const;
    void setNormalize(bool normalize);
    bool getNormalize() const;
    void setUseGaussianBlur(bool useBlur);
    bool getUseGaussianBlur() const;
    void setGaussianSigma(double sigma);
    double getGaussianSigma() const;
    
    /**
     * @brief Obtiene información sobre el filtro
     * @return String con información del filtro
     */
    std::string getInfo() const;
};

/**
 * @brief Filtro de detección de bordes usando el operador de Sobel con templates
 * 
 * Esta clase implementa el algoritmo de Sobel para detectar bordes
 * en imágenes usando templates para mayor flexibilidad en tipos de datos.
 * 
 * @tparam T Tipo de datos para los cálculos (uint8_t, float, double)
 * @tparam KernelSize Tamaño del kernel (3 para Sobel estándar)
 * 
 * Características:
 * - Implementación manual del algoritmo (no usa OpenCV para el filtro)
 * - Soporte para diferentes tipos de datos mediante templates
 * - Configuración flexible
 * - Manejo robusto de errores
 * - Optimizaciones de rendimiento
 * 
 * @example
 * SobelFilterTemplate<uint8_t> filter;  // Para imágenes 8-bit
 * SobelFilterTemplate<float> filter;    // Para cálculos de precisión
 * 
 * @see https://en.wikipedia.org/wiki/Sobel_operator
 */
template<typename T = uint8_t, int KernelSize = 3>
class SobelFilterTemplate {
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");
    static_assert(KernelSize > 0 && KernelSize % 2 == 1, "KernelSize must be positive odd number");
    
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
    
    static constexpr int KERNEL_OFFSET = KernelSize / 2;
    static constexpr T MAX_VALUE = std::numeric_limits<T>::max();
    static constexpr T MIN_VALUE = std::numeric_limits<T>::min();
    
    FilterConfig config_;
    std::unique_ptr<cv::Mat> cached_result_;
    
    // Métodos privados
    void validateInput(const cv::Mat& input) const;
    cv::Mat convertToGrayscale(const cv::Mat& input) const;
    T applyKernel(const cv::Mat& image, int row, int col, 
                 const std::array<std::array<int, 3>, 3>& kernel) const;
    double calculateMagnitude(T gx, T gy) const;
    T normalizeValue(double value) const;
    uchar convertToOutputType(T value) const;

public:
    /**
     * @brief Constructor con configuración opcional
     * @param config Configuración del filtro
     */
    explicit SobelFilterTemplate(const FilterConfig& config = FilterConfig{});
    
    /**
     * @brief Constructor por movimiento
     */
    SobelFilterTemplate(SobelFilterTemplate&& other) noexcept = default;
    
    /**
     * @brief Operador de asignación por movimiento
     */
    SobelFilterTemplate& operator=(SobelFilterTemplate&& other) noexcept = default;
    
    /**
     * @brief Destructor virtual
     */
    virtual ~SobelFilterTemplate() = default;
    
    /**
     * @brief Aplica el filtro Sobel a una imagen
     * @param input Imagen de entrada
     * @return Imagen procesada o std::nullopt si hay error
     */
    std::optional<cv::Mat> applyFilter(const cv::Mat& input) const;
    
    /**
     * @brief Aplica el filtro Sobel con umbral
     * @param input Imagen de entrada
     * @param threshold Umbral (usa el configurado si es -1)
     * @return Imagen procesada con umbral o std::nullopt si hay error
     */
    std::optional<cv::Mat> applyFilterWithThreshold(const cv::Mat& input, int threshold = -1) const;
    
    // Getters y setters para configuración
    void setThreshold(int threshold);
    int getThreshold() const;
    void setNormalize(bool normalize);
    bool getNormalize() const;
    void setUseGaussianBlur(bool useBlur);
    bool getUseGaussianBlur() const;
    void setGaussianSigma(double sigma);
    double getGaussianSigma() const;
    
    /**
     * @brief Obtiene información sobre el filtro
     * @return String con información del filtro
     */
    std::string getInfo() const;
    
    /**
     * @brief Obtiene el tipo de datos usado
     * @return String con el nombre del tipo
     */
    static std::string getTypeName();
};

// Incluir implementaciones de templates (comentado por ahora)
// #include "sobel_filter.hpp"

#endif // SOBEL_FILTER_H 