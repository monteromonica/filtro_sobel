#!/bin/bash

# Script para asegurar que existe la imagen de prueba
# Filtro Sobel - Prueba Tecnica Photonicsens

echo "=== Verificación de Imagen de Prueba ==="

# Verificar si la imagen existe
if [ -f "images/test_image.jpg" ] && [ "$1" != "--force" ]; then
    file_size=$(du -h "images/test_image.jpg" | cut -f1)
    echo "✅ Imagen de prueba encontrada: images/test_image.jpg ($file_size)"
    exit 0
fi

# Si no existe o se fuerza regeneración, crear la imagen
echo "🔄 Generando imagen de prueba..."

# Verificar Docker
if ! command -v docker &> /dev/null; then
    echo "❌ Error: Docker no está disponible"
    exit 1
fi

# Crear directorio build limpio
echo "Paso 1: Preparar entorno..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "rm -rf build && mkdir -p build"

# Configurar y compilar
echo "Paso 2: Configurar y compilar..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cmake .. && make"

# Generar imagen de prueba
echo "Paso 3: Generar imagen de prueba..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && ./test_sobel_no_gui"

# Copiar solo la imagen de prueba
echo "Paso 4: Copiar imagen..."
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cp test_image.jpg /workspace/images/ 2>/dev/null || true"

# Verificar que se copió correctamente
if [ -f "images/test_image.jpg" ]; then
    file_size=$(du -h "images/test_image.jpg" | cut -f1)
    echo "✅ Imagen de prueba generada exitosamente: images/test_image.jpg ($file_size)"
    exit 0
else
    echo "❌ Error: No se pudo generar la imagen de prueba"
    exit 1
fi 