#ifndef FILTER_FACTORY_H
#define FILTER_FACTORY_H

#include "edge_detection_strategy.h"
#include <memory>
#include <vector>
#include <string>

/**
 * @brief Factory para crear diferentes tipos de filtros de detección de bordes
 * 
 * Este patrón Factory centraliza la creación de diferentes algoritmos
 * de detección de bordes, proporcionando una interfaz unificada para
 * crear instancias de diferentes estrategias.
 * 
 * @example
 * auto filter = FilterFactory::createFilter(FilterFactory::FilterType::SOBEL);
 * auto result = filter->detectEdges(input_image);
 * 
 * @see https://en.wikipedia.org/wiki/Factory_pattern
 */
class FilterFactory {
public:
    /**
     * @brief Tipos de filtros disponibles
     */
    enum class FilterType {
        SOBEL_BASIC,        // Filtro Sobel básico
        SOBEL_IMPROVED,     // Filtro Sobel mejorado (C++ moderno)
        SOBEL_OMP,          // Filtro Sobel con OpenMP
        SOBEL_PTHREAD,      // Filtro Sobel con pThreads
        CANNY               // Filtro Canny (futuro)
    };
    
    /**
     * @brief Crea una instancia del filtro especificado
     * @param type Tipo de filtro a crear
     * @return Puntero único al filtro creado o nullptr si no se pudo crear
     */
    static std::unique_ptr<EdgeDetectionStrategy> createFilter(FilterType type);
    
    /**
     * @brief Crea un filtro a partir de su nombre
     * @param name Nombre del filtro ("sobel", "sobel_omp", etc.)
     * @return Puntero único al filtro creado o nullptr si no se pudo crear
     */
    static std::unique_ptr<EdgeDetectionStrategy> createFilter(const std::string& name);
    
    /**
     * @brief Obtiene todos los tipos de filtros disponibles
     * @return Vector con todos los tipos de filtros
     */
    static std::vector<FilterType> getAvailableFilterTypes();
    
    /**
     * @brief Obtiene los nombres de todos los filtros disponibles
     * @return Vector con los nombres de los filtros
     */
    static std::vector<std::string> getAvailableFilterNames();
    
    /**
     * @brief Convierte un tipo de filtro a string
     * @param type Tipo de filtro
     * @return String con el nombre del tipo
     */
    static std::string filterTypeToString(FilterType type);
    
    /**
     * @brief Convierte un string a tipo de filtro
     * @param name Nombre del filtro
     * @return Tipo de filtro correspondiente
     */
    static FilterType stringToFilterType(const std::string& name);
    
    /**
     * @brief Verifica si un tipo de filtro está disponible
     * @param type Tipo de filtro a verificar
     * @return true si está disponible, false en caso contrario
     */
    static bool isFilterTypeAvailable(FilterType type);
    
    /**
     * @brief Obtiene información sobre todos los filtros disponibles
     * @return String con información de todos los filtros
     */
    static std::string getAvailableFiltersInfo();
    
    /**
     * @brief Registra un nuevo tipo de filtro (para extensibilidad)
     * @param type Tipo de filtro
     * @param name Nombre del filtro
     * @param description Descripción del filtro
     */
    static void registerFilterType(FilterType type, const std::string& name, const std::string& description);

private:
    /**
     * @brief Información de un tipo de filtro
     */
    struct FilterInfo {
        FilterType type;
        std::string name;
        std::string description;
        bool available;
        
        FilterInfo(FilterType t, const std::string& n, const std::string& desc, bool avail = true)
            : type(t), name(n), description(desc), available(avail) {}
    };
    
    // Mapa de tipos de filtros registrados
    static std::vector<FilterInfo> registered_filters_;
    
    /**
     * @brief Inicializa los filtros registrados por defecto
     */
    static void initializeDefaultFilters();
};

#endif // FILTER_FACTORY_H 