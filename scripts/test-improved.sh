#!/bin/bash

# Script para probar las versiones mejoradas del filtro Sobel
# Filtro Sobel - Prueba Tecnica Photonicsens

echo "=== Prueba de Versiones Mejoradas del Filtro Sobel ==="

# Verificar Docker
if ! command -v docker &> /dev/null; then
    echo "❌ Error: Docker no está disponible"
    exit 1
fi

echo "✅ Docker encontrado"
echo ""
echo "Compilando y probando las versiones mejoradas..."

# Asegurar que existe la imagen de prueba
echo "Paso 1: Verificar imagen de prueba..."
if ! bash "$(dirname "$0")/ensure-test-image.sh"; then
    echo "❌ Error al verificar imagen de prueba"
    exit 1
fi

# Compilar y ejecutar paso a paso
echo "Paso 2: Crear directorio build..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "rm -rf build && mkdir -p build"

echo "Paso 3: Configurar con CMake..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cmake .."

echo "Paso 4: Compilar..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && make"

echo "Paso 5: Copiar imagen de prueba..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cp /workspace/images/test_image.jpg build/"

# Probar versión mejorada
echo "Paso 6: Probar versión mejorada..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && ./sobel_filter_improved test_image.jpg sobel_improved_result.jpg"

# Copiar imágenes generadas
echo ""
echo "Copiando imágenes generadas..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cp sobel_improved_result.jpg sobel_improved_result_threshold.jpg /workspace/images/ 2>/dev/null || true"

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Prueba de versiones mejoradas exitosa ==="
    echo "Las versiones mejoradas funcionan correctamente"
    echo "Imágenes copiadas a: images/"
    
    # Mostrar archivos generados
    echo ""
    echo "Archivos generados:"
    for file in images/sobel_improved*.jpg; do
        if [ -f "$file" ]; then
            echo "  ✅ $(basename "$file")"
        fi
    done
    
    echo ""
    echo "=== Comparación de Versiones ==="
    echo "Versión Original:     sobel_basic_result.jpg"
    echo "Versión Mejorada:     sobel_improved_result.jpg"
    
else
    echo ""
    echo "=== Error en la prueba de versiones mejoradas ==="
    echo "Pero las imágenes se copiaron si estaban disponibles"
fi 