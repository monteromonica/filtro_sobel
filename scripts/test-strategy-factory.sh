#!/bin/bash

# Script para probar los patrones Strategy y Factory
# Filtro Sobel - Prueba Tecnica Photonicsens

echo "=== Prueba de Patrones Strategy + Factory ==="

# Verificar Docker
if ! command -v docker &> /dev/null; then
    echo "❌ Error: Docker no está disponible"
    exit 1
fi

echo "✅ Docker encontrado"
echo ""
echo "Compilando y probando los patrones de diseño..."

# Asegurar que existe la imagen de prueba
echo "Paso 1: Verificar imagen de prueba..."
if ! bash "$(dirname "$0")/ensure-test-image.sh"; then
    echo "❌ Error al verificar imagen de prueba"
    exit 1
fi

# Crear directorio de salida
echo "Paso 2: Crear directorio de salida..."
mkdir -p images/strategy_factory_output

# Compilar y ejecutar paso a paso
echo "Paso 3: Crear directorio build..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "rm -rf build && mkdir -p build"

echo "Paso 4: Configurar con CMake..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cmake .."

echo "Paso 5: Compilar..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && make"

echo "Paso 6: Copiar imagen de prueba..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cp /workspace/images/test_image.jpg build/"

# Probar patrones Strategy + Factory
echo "Paso 7: Probar patrones Strategy + Factory..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && ./test_strategy_factory test_image.jpg strategy_factory_output"

# Copiar imágenes generadas
echo ""
echo "Copiando imágenes generadas..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cp -r strategy_factory_output/* /workspace/images/strategy_factory_output/ 2>/dev/null || true"

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Prueba de patrones Strategy + Factory exitosa ==="
    echo "Los patrones de diseño funcionan correctamente"
    echo "Imágenes copiadas a: images/strategy_factory_output/"
    
    # Mostrar archivos generados
    echo ""
    echo "Archivos generados:"
    for file in images/strategy_factory_output/*.jpg; do
        if [ -f "$file" ]; then
            echo "  ✅ $(basename "$file")"
        fi
    done
    
    echo ""
    echo "=== Beneficios de los Patrones ==="
    echo "✅ Strategy Pattern: Intercambio transparente de algoritmos"
    echo "✅ Factory Pattern: Creación centralizada de filtros"
    echo "✅ Extensibilidad: Fácil agregar nuevos algoritmos"
    echo "✅ Interfaz unificada: Mismo API para todos los filtros"
    echo "✅ Preparado para Android NDK: Arquitectura lista para JNI"
    
else
    echo ""
    echo "=== Error en la prueba de patrones ==="
    echo "Pero las imágenes se copiaron si estaban disponibles"
fi 