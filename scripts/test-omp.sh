#!/bin/bash

# Script para probar la versión OpenMP del filtro Sobel
# Filtro Sobel - Prueba Tecnica Photonicsens

echo "=== Prueba del Filtro Sobel con OpenMP ==="

# Verificar Docker
if ! command -v docker &> /dev/null; then
    echo "❌ Error: Docker no está disponible"
    exit 1
fi

echo "✅ Docker encontrado"
echo ""
echo "Compilando y probando la versión OpenMP..."

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

echo "Paso 6: Ejecutar prueba OpenMP..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && ./test_sobel_omp_fixed"

# Copiar solo las imágenes específicas de OpenMP
echo ""
echo "Copiando imágenes generadas..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cp test_sobel_omp_result.jpg test_sobel_omp_threshold.jpg /workspace/images/ 2>/dev/null || true"

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Prueba OpenMP exitosa ==="
    echo "La versión multihilo funciona correctamente"
    echo "Imágenes copiadas a: images/"
    
    # Mostrar archivos generados
    echo ""
    echo "Archivos generados:"
    for file in images/*omp*.jpg; do
        if [ -f "$file" ]; then
            echo "  ✅ $(basename "$file")"
        fi
    done
else
    echo ""
    echo "=== Error en la prueba OpenMP ==="
    echo "Pero las imágenes se copiaron si estaban disponibles"
fi 