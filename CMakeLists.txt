cmake_minimum_required(VERSION 3.10)
project(SobelFilter)

# Configurar C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar OpenCV
find_package(OpenCV REQUIRED)

# Mostrar información de OpenCV encontrado
if(OpenCV_FOUND)
    message(STATUS "OpenCV encontrado:")
    message(STATUS "    Version: ${OpenCV_VERSION}")
    message(STATUS "    Libraries: ${OpenCV_LIBS}")
    message(STATUS "    Include path: ${OpenCV_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "OpenCV no encontrado")
endif()

# Encontrar OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP encontrado:")
    message(STATUS "    Flags: ${OpenMP_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP no encontrado - usando versión secuencial")
endif()

# Configurar directorio de includes
include_directories(include)

# Crear ejecutables
add_executable(sobel_filter src/sobel_filter.cpp)
add_executable(sobel_filter_improved src/sobel_filter_improved.cpp)
add_executable(test_strategy_factory src/test_strategy_factory.cpp src/filter_factory.cpp src/sobel_filter_improved_lib.cpp)
# add_executable(sobel_filter_template src/sobel_filter_template.cpp)  # Comentado por problemas de compilación
add_executable(sobel_filter_omp src/sobel_filter_omp.cpp)
add_executable(sobel_filter_pthread src/sobel_filter_pthread.cpp)
add_executable(test_sobel tests/test_sobel.cpp)
add_executable(test_sobel_no_gui tests/test_sobel_no_gui.cpp)
add_executable(test_sobel_omp tests/test_sobel_omp.cpp)
add_executable(test_sobel_omp_fixed tests/test_sobel_omp_fixed.cpp)

# Vincular con OpenCV
target_link_libraries(sobel_filter ${OpenCV_LIBS})
target_link_libraries(sobel_filter_improved ${OpenCV_LIBS})
target_link_libraries(test_strategy_factory ${OpenCV_LIBS})
# target_link_libraries(sobel_filter_template ${OpenCV_LIBS})  # Comentado por problemas de compilación
target_link_libraries(sobel_filter_omp ${OpenCV_LIBS})
target_link_libraries(sobel_filter_pthread ${OpenCV_LIBS})
target_link_libraries(test_sobel ${OpenCV_LIBS})
target_link_libraries(test_sobel_no_gui ${OpenCV_LIBS})
target_link_libraries(test_sobel_omp ${OpenCV_LIBS})
target_link_libraries(test_sobel_omp_fixed ${OpenCV_LIBS})

# Vincular con pThreads
target_link_libraries(sobel_filter_pthread pthread)

# Configuraciones adicionales para Windows
if(WIN32)
    target_compile_definitions(sobel_filter PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Configuraciones de compilación
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(sobel_filter PRIVATE -g -O0 -Wall -Wextra)
else()
    target_compile_options(sobel_filter PRIVATE -O3 -Wall -Wextra)
endif() 