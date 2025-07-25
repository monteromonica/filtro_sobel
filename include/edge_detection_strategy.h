#ifndef EDGE_DETECTION_STRATEGY_H
#define EDGE_DETECTION_STRATEGY_H

#include <opencv2/opencv.hpp>
#include <optional>
#include <string>
#include <memory>

/**
 * @brief Estrategia base para algoritmos de detección de bordes
 * 
 * Este patrón Strategy permite intercambiar diferentes algoritmos
 * de detección de bordes de forma transparente. Cada implementación
 * debe proporcionar los métodos para detectar bordes y obtener
 * información sobre el algoritmo.
 * 
 * @example
 * std::unique_ptr<EdgeDetectionStrategy> strategy = 
 *     std::make_unique<SobelStrategy>();
 * auto result = strategy->detectEdges(input_image);
 * 
 * @see https://en.wikipedia.org/wiki/Strategy_pattern
 */
class EdgeDetectionStrategy {
public:
    /**
     * @brief Destructor virtual
     */
    virtual ~EdgeDetectionStrategy() = default;
    
    /**
     * @brief Detecta bordes en una imagen de entrada
     * @param input Imagen de entrada (puede ser color o escala de grises)
     * @return Imagen con bordes detectados o std::nullopt si hay error
     */
    virtual std::optional<cv::Mat> detectEdges(const cv::Mat& input) = 0;
    
    /**
     * @brief Detecta bordes con umbral aplicado
     * @param input Imagen de entrada
     * @param threshold Umbral para binarización (0-255)
     * @return Imagen binaria con bordes o std::nullopt si hay error
     */
    virtual std::optional<cv::Mat> detectEdgesWithThreshold(const cv::Mat& input, int threshold = 128) = 0;
    
    /**
     * @brief Obtiene el nombre del algoritmo
     * @return String con el nombre del algoritmo
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Obtiene información detallada del algoritmo
     * @return String con información del algoritmo y configuración
     */
    virtual std::string getInfo() const = 0;
    
    /**
     * @brief Verifica si el algoritmo está disponible
     * @return true si el algoritmo está disponible, false en caso contrario
     */
    virtual bool isAvailable() const = 0;
    
    /**
     * @brief Obtiene el tiempo de ejecución del último procesamiento
     * @return Tiempo en milisegundos, -1 si no se ha ejecutado
     */
    virtual double getLastExecutionTime() const = 0;
    
    /**
     * @brief Resetea las estadísticas de ejecución
     */
    virtual void resetStats() = 0;
};

#endif // EDGE_DETECTION_STRATEGY_H 