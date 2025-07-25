#!/bin/bash

# Script para limpiar imágenes antiguas
# Filtro Sobel - Prueba Tecnica Photonicsens

echo "=== Limpieza de Imágenes ==="

# Contar imágenes antes de limpiar
image_count=$(find images -name "*.jpg" 2>/dev/null | wc -l)

if [ $image_count -eq 0 ]; then
    echo "No hay imágenes para limpiar"
    echo "✅ Limpieza completada"
    exit 0
fi

echo "Eliminando imágenes antiguas..."

# Eliminar todas las imágenes .jpg
find images -name "*.jpg" -delete 2>/dev/null

echo "Imágenes eliminadas: $image_count"
echo "Directorio images limpio"
echo ""
echo "✅ Limpieza completada" 