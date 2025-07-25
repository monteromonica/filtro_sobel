#!/bin/bash

# Script maestro para ejecutar todas las pruebas del filtro Sobel
# Filtro Sobel - Prueba Tecnica Photonicsens

echo "=== PRUEBA COMPLETA DEL FILTRO SOBEL ==="
echo "Este script ejecutará todas las versiones y verificará que todo funcione correctamente"

# Paso 1: Limpiar imágenes antiguas
echo ""
echo "Paso 1: Limpieza de imágenes antiguas..."
bash "$(dirname "$0")/clean-images.sh"

# Paso 2: Asegurar que existe la imagen de prueba
echo ""
echo "Paso 2: Verificando imagen de prueba..."
if ! bash "$(dirname "$0")/ensure-test-image.sh"; then
    echo "❌ Error al verificar imagen de prueba"
    exit 1
fi

# Paso 3: Ejecutar todas las versiones
echo ""
echo "Paso 3: Ejecutando todas las versiones..."

# Ejecutar prueba básica (requisito mínimo)
echo "Ejecutando prueba básica..."
if ! bash "$(dirname "$0")/test-basic.sh"; then
    echo "❌ Error en prueba básica"
    exit 1
fi

# Ejecutar prueba OpenMP
echo "Ejecutando prueba OpenMP..."
if ! bash "$(dirname "$0")/test-omp.sh"; then
    echo "❌ Error en prueba OpenMP"
    exit 1
fi

# Ejecutar prueba pThreads
echo "Ejecutando prueba pThreads..."
if ! bash "$(dirname "$0")/test-pthread.sh"; then
    echo "❌ Error en prueba pThreads"
    exit 1
fi

# Paso 4: Verificación final
echo ""
echo "=== VERIFICACIÓN FINAL ==="

# Verificar que todas las imágenes se generaron correctamente
expected_files=(
    "test_image.jpg"
    "sobel_basic_result.jpg"
    "sobel_basic_result_threshold.jpg"
    "test_sobel_omp_result.jpg"
    "test_sobel_omp_threshold.jpg"
    "sobel_pthread_result.jpg"
    "sobel_pthread_result_threshold.jpg"
)

missing_files=()
found_files=()

for file in "${expected_files[@]}"; do
    if [ -f "images/$file" ]; then
        found_files+=("$file")
        echo "✅ $file"
    else
        missing_files+=("$file")
        echo "❌ $file - faltante"
    fi
done

echo ""
echo "=== RESUMEN FINAL ==="
echo "Archivos encontrados: ${#found_files[@]}/${#expected_files[@]}"

if [ ${#missing_files[@]} -eq 0 ]; then
    echo ""
    echo "🎉 ¡TODAS LAS PRUEBAS EXITOSAS!"
    echo "✅ Todas las versiones del filtro Sobel funcionan correctamente"
    echo "✅ Todas las imágenes se generaron y copiaron automáticamente"
    echo "✅ La configuración Docker está funcionando perfectamente"
else
    echo ""
    echo "⚠️  Algunos archivos faltan:"
    for file in "${missing_files[@]}"; do
        echo "  - $file"
    done
fi

echo ""
echo "=== INFORMACIÓN ADICIONAL ==="
echo "Directorio de imágenes: $(pwd)/images"
echo "Total de archivos: $(find images -name "*.jpg" 2>/dev/null | wc -l)"

echo ""
echo "✅ Proceso completado" 