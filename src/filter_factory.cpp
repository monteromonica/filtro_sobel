// =============================================================
//  FILTER_FACTORY.CPP
//  -----------------------------------------------------------
//  Implementa el patrón Factory para la creación centralizada
//  de filtros de detección de bordes. Facilita la extensibilidad
//  y la integración con Android NDK/JNI.
// =============================================================

#include "filter_factory.h"
#include "sobel_strategies.cpp"
#include <algorithm>
#include <stdexcept>

// Inicializar el vector estático de filtros registrados
std::vector<FilterFactory::FilterInfo> FilterFactory::registered_filters_;

/**
 * @brief Inicializa los filtros registrados por defecto
 */
void FilterFactory::initializeDefaultFilters() {
    if (registered_filters_.empty()) {
        registered_filters_.emplace_back(FilterType::SOBEL_BASIC, "sobel_basic", 
                                       "Filtro Sobel básico - Implementación secuencial estándar");
        registered_filters_.emplace_back(FilterType::SOBEL_IMPROVED, "sobel_improved", 
                                       "Filtro Sobel mejorado - C++ moderno con manejo de errores");
        registered_filters_.emplace_back(FilterType::SOBEL_OMP, "sobel_omp", 
                                       "Filtro Sobel OpenMP - Paralelización automática");
        registered_filters_.emplace_back(FilterType::SOBEL_PTHREAD, "sobel_pthread", 
                                       "Filtro Sobel pThreads - Control manual de hilos");
        registered_filters_.emplace_back(FilterType::CANNY, "canny", 
                                       "Filtro Canny - Detección de bordes avanzada", false);
    }
}

/**
 * @brief Crea una instancia del filtro especificado
 */
std::unique_ptr<EdgeDetectionStrategy> FilterFactory::createFilter(FilterType type) {
    initializeDefaultFilters();
    
    switch (type) {
        case FilterType::SOBEL_BASIC:
            return std::make_unique<SobelBasicStrategy>();
            
        case FilterType::SOBEL_IMPROVED:
            return std::make_unique<SobelImprovedStrategy>();
            
        case FilterType::SOBEL_OMP:
            return std::make_unique<SobelOMPStrategy>();
            
        case FilterType::SOBEL_PTHREAD:
            return std::make_unique<SobelPThreadStrategy>();
            
        case FilterType::CANNY:
            // TODO: Implementar cuando se necesite
            std::cerr << "Filtro Canny no implementado aún" << std::endl;
            return nullptr;
            
        default:
            std::cerr << "Tipo de filtro desconocido" << std::endl;
            return nullptr;
    }
}

/**
 * @brief Crea un filtro a partir de su nombre
 */
std::unique_ptr<EdgeDetectionStrategy> FilterFactory::createFilter(const std::string& name) {
    FilterType type = stringToFilterType(name);
    return createFilter(type);
}

/**
 * @brief Obtiene todos los tipos de filtros disponibles
 */
std::vector<FilterFactory::FilterType> FilterFactory::getAvailableFilterTypes() {
    initializeDefaultFilters();
    
    std::vector<FilterType> available_types;
    for (const auto& filter : registered_filters_) {
        if (filter.available) {
            available_types.push_back(filter.type);
        }
    }
    return available_types;
}

/**
 * @brief Obtiene los nombres de todos los filtros disponibles
 */
std::vector<std::string> FilterFactory::getAvailableFilterNames() {
    initializeDefaultFilters();
    
    std::vector<std::string> available_names;
    for (const auto& filter : registered_filters_) {
        if (filter.available) {
            available_names.push_back(filter.name);
        }
    }
    return available_names;
}

/**
 * @brief Convierte un tipo de filtro a string
 */
std::string FilterFactory::filterTypeToString(FilterType type) {
    initializeDefaultFilters();
    
    for (const auto& filter : registered_filters_) {
        if (filter.type == type) {
            return filter.name;
        }
    }
    return "unknown";
}

/**
 * @brief Convierte un string a tipo de filtro
 */
FilterFactory::FilterType FilterFactory::stringToFilterType(const std::string& name) {
    initializeDefaultFilters();
    
    for (const auto& filter : registered_filters_) {
        if (filter.name == name) {
            return filter.type;
        }
    }
    
    // Si no se encuentra, intentar con nombres alternativos
    if (name == "sobel" || name == "basic") {
        return FilterType::SOBEL_BASIC;
    } else if (name == "improved" || name == "modern") {
        return FilterType::SOBEL_IMPROVED;
    } else if (name == "omp" || name == "openmp") {
        return FilterType::SOBEL_OMP;
    } else if (name == "pthread" || name == "pthreads") {
        return FilterType::SOBEL_PTHREAD;
    }
    
    // Por defecto, retornar SOBEL_BASIC
    return FilterType::SOBEL_BASIC;
}

/**
 * @brief Verifica si un tipo de filtro está disponible
 */
bool FilterFactory::isFilterTypeAvailable(FilterType type) {
    initializeDefaultFilters();
    
    for (const auto& filter : registered_filters_) {
        if (filter.type == type) {
            return filter.available;
        }
    }
    return false;
}

/**
 * @brief Obtiene información sobre todos los filtros disponibles
 */
std::string FilterFactory::getAvailableFiltersInfo() {
    initializeDefaultFilters();
    
    std::string info = "=== FILTROS DISPONIBLES ===\n\n";
    
    for (const auto& filter : registered_filters_) {
        info += "• " + filter.name + " (" + filterTypeToString(filter.type) + ")\n";
        info += "  " + filter.description + "\n";
        info += "  Estado: " + std::string(filter.available ? "✅ Disponible" : "❌ No disponible") + "\n\n";
    }
    
    return info;
}

/**
 * @brief Registra un nuevo tipo de filtro
 */
void FilterFactory::registerFilterType(FilterType type, const std::string& name, const std::string& description) {
    initializeDefaultFilters();
    
    // Verificar si ya existe
    for (auto& filter : registered_filters_) {
        if (filter.type == type || filter.name == name) {
            // Actualizar información existente
            filter.name = name;
            filter.description = description;
            filter.available = true;
            return;
        }
    }
    
    // Agregar nuevo filtro
    registered_filters_.emplace_back(type, name, description, true);
} 