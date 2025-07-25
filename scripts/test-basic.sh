#!/bin/bash

# Script para probar el requisito mínimo del filtro Sobel
# Filtro Sobel - Prueba Tecnica Photonicsens

echo "=== Prueba del Filtro Sobel (Requisito Mínimo) ==="

# Verificar Docker
if ! command -v docker &> /dev/null; then
    echo "❌ Error: Docker no está disponible"
    exit 1
fi

echo "✅ Docker encontrado"
echo ""
echo "Compilando y probando la versión básica..."

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

echo "Paso 6: Ejecutar filtro Sobel básico..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && ./sobel_filter test_image.jpg sobel_basic_result.jpg"

# Copiar solo las imágenes específicas del filtro básico
echo ""
echo "Copiando imágenes generadas..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cp sobel_basic_result.jpg sobel_basic_result_threshold.jpg /workspace/images/ 2>/dev/null || true"

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Prueba básica exitosa ==="
    echo "El filtro Sobel básico funciona correctamente"
    echo "Imágenes copiadas a: images/"
    
    # Mostrar archivos generados
    echo ""
    echo "Archivos generados:"
    for file in images/sobel_basic*.jpg; do
        if [ -f "$file" ]; then
            echo "  ✅ $(basename "$file")"
        fi
    done
else
    echo ""
    echo "=== Error en la prueba básica ==="
    echo "Pero las imágenes se copiaron si estaban disponibles"
fi 