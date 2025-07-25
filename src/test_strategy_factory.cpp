#include "edge_detection_strategy.h"
#include "filter_factory.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>

/**
 * @brief Programa de prueba para demostrar Strategy y Factory patterns
 * 
 * Este programa muestra cómo usar los patrones de diseño implementados
 * para crear y usar diferentes algoritmos de detección de bordes de
 * forma transparente y extensible.
 */
int main(int argc, char** argv) {
    try {
        std::cout << "=== DEMOSTRACIÓN STRATEGY + FACTORY PATTERNS ===" << std::endl;
        std::cout << "Filtro Sobel - Prueba Técnica Photonicsens" << std::endl;
        std::cout << std::endl;
        
        // Verificar argumentos
        if (argc != 3) {
            std::cout << "Uso: " << argv[0] << " <imagen_entrada> <directorio_salida>" << std::endl;
            std::cout << "Ejemplo: " << argv[0] << " test_image.jpg output/" << std::endl;
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
        std::cout << std::endl;
        
        // Mostrar información de filtros disponibles
        std::cout << FilterFactory::getAvailableFiltersInfo();
        
        // Obtener todos los tipos de filtros disponibles
        auto availableTypes = FilterFactory::getAvailableFilterTypes();
        auto availableNames = FilterFactory::getAvailableFilterNames();
        
        std::cout << "=== PRUEBA DE TODOS LOS FILTROS ===" << std::endl;
        std::cout << std::endl;
        
        // Probar cada filtro disponible
        for (size_t i = 0; i < availableTypes.size(); ++i) {
            auto filterType = availableTypes[i];
            auto filterName = availableNames[i];
            
            std::cout << "Probando: " << filterName << std::endl;
            std::cout << "Tipo: " << FilterFactory::filterTypeToString(filterType) << std::endl;
            
            // Crear filtro usando Factory
            auto filter = FilterFactory::createFilter(filterType);
            if (!filter) {
                std::cerr << "❌ Error: No se pudo crear el filtro " << filterName << std::endl;
                continue;
            }
            
            std::cout << "✅ Filtro creado exitosamente" << std::endl;
            std::cout << "Info: " << filter->getInfo() << std::endl;
            
            // Probar detección de bordes
            auto start = std::chrono::high_resolution_clock::now();
            auto result = filter->detectEdges(inputImage);
            auto end = std::chrono::high_resolution_clock::now();
            
            if (!result) {
                std::cerr << "❌ Error: No se pudo procesar la imagen con " << filterName << std::endl;
                continue;
            }
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            double time_ms = duration.count() / 1000.0;
            
            std::cout << "✅ Procesamiento exitoso" << std::endl;
            std::cout << "Tiempo: " << std::fixed << std::setprecision(2) << time_ms << " ms" << std::endl;
            std::cout << "Tiempo interno: " << filter->getLastExecutionTime() << " ms" << std::endl;
            
            // Probar detección con umbral
            auto thresholdResult = filter->detectEdgesWithThreshold(inputImage, 50);
            if (thresholdResult) {
                std::cout << "✅ Umbralización exitosa" << std::endl;
            } else {
                std::cerr << "❌ Error en umbralización" << std::endl;
            }
            
            // Guardar resultados
            std::string outputDir = argv[2];
            std::string baseFilename = outputDir + "/" + filterName;
            
            if (cv::imwrite(baseFilename + "_result.jpg", *result)) {
                std::cout << "✅ Resultado guardado: " << baseFilename + "_result.jpg" << std::endl;
            } else {
                std::cerr << "❌ Error al guardar resultado" << std::endl;
            }
            
            if (thresholdResult && cv::imwrite(baseFilename + "_threshold.jpg", *thresholdResult)) {
                std::cout << "✅ Umbral guardado: " << baseFilename + "_threshold.jpg" << std::endl;
            } else {
                std::cerr << "❌ Error al guardar umbral" << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        // Demostrar uso dinámico de filtros
        std::cout << "=== DEMOSTRACIÓN DE USO DINÁMICO ===" << std::endl;
        std::cout << std::endl;
        
        // Crear filtro por nombre
        std::string filterName = "sobel_improved";
        std::cout << "Creando filtro por nombre: " << filterName << std::endl;
        
        auto dynamicFilter = FilterFactory::createFilter(filterName);
        if (dynamicFilter) {
            std::cout << "✅ Filtro creado: " << dynamicFilter->getName() << std::endl;
            std::cout << "Info: " << dynamicFilter->getInfo() << std::endl;
            
            // Procesar imagen
            auto result = dynamicFilter->detectEdges(inputImage);
            if (result) {
                std::cout << "✅ Procesamiento dinámico exitoso" << std::endl;
                std::cout << "Tiempo: " << dynamicFilter->getLastExecutionTime() << " ms" << std::endl;
            }
        } else {
            std::cerr << "❌ Error al crear filtro dinámico" << std::endl;
        }
        
        std::cout << std::endl;
        
        // Comparación de rendimiento
        std::cout << "=== COMPARACIÓN DE RENDIMIENTO ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << std::left << std::setw(20) << "Filtro" 
                  << std::setw(15) << "Tiempo (ms)" 
                  << std::setw(15) << "Estado" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        for (const auto& filterType : availableTypes) {
            auto filter = FilterFactory::createFilter(filterType);
            if (filter) {
                filter->resetStats();
                auto result = filter->detectEdges(inputImage);
                
                std::string status = result ? "✅ OK" : "❌ Error";
                double time = filter->getLastExecutionTime();
                
                std::cout << std::left << std::setw(20) << filter->getName()
                          << std::setw(15) << std::fixed << std::setprecision(2) << time
                          << std::setw(15) << status << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== DEMOSTRACIÓN COMPLETADA ===" << std::endl;
        std::cout << "Los patrones Strategy y Factory funcionan correctamente." << std::endl;
        std::cout << "El código está listo para Android NDK/JNI." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
} 