# Script para limpiar imágenes antiguas
# Filtro Sobel - Prueba Tecnica Photonicsens

Write-Host "=== Limpieza de Imágenes ===" -ForegroundColor Green

# Verificar si existe el directorio images
if (Test-Path "images") {
    Write-Host "Eliminando imágenes antiguas..." -ForegroundColor Yellow
    
    # Contar archivos antes de eliminar
    $filesBefore = Get-ChildItem images\*.jpg -ErrorAction SilentlyContinue | Measure-Object | Select-Object -ExpandProperty Count
    
    # Eliminar todas las imágenes .jpg
    Remove-Item images\*.jpg -Force -ErrorAction SilentlyContinue
    
    # Contar archivos después de eliminar
    $filesAfter = Get-ChildItem images\*.jpg -ErrorAction SilentlyContinue | Measure-Object | Select-Object -ExpandProperty Count
    
    Write-Host "Imágenes eliminadas: $($filesBefore - $filesAfter)" -ForegroundColor Green
    Write-Host "Directorio images limpio" -ForegroundColor Green
} else {
    Write-Host "Directorio images no existe, creándolo..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "images" -Force | Out-Null
    Write-Host "Directorio images creado" -ForegroundColor Green
}

Write-Host ""
Write-Host "✅ Limpieza completada" -ForegroundColor Green 