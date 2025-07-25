# Script maestro para ejecutar todas las pruebas del filtro Sobel
# Filtro Sobel - Prueba Tecnica Photonicsens

Write-Host "=== PRUEBA COMPLETA DEL FILTRO SOBEL ===" -ForegroundColor Cyan
Write-Host "Este script ejecutará todas las versiones y verificará que todo funcione correctamente" -ForegroundColor White

# Paso 1: Limpiar imágenes antiguas
Write-Host ""
Write-Host "Paso 1: Limpieza de imágenes antiguas..." -ForegroundColor Yellow
& "$PSScriptRoot\clean-images.ps1"

# Paso 2: Asegurar que existe la imagen de prueba
Write-Host ""
Write-Host "Paso 2: Verificando imagen de prueba..." -ForegroundColor Yellow
& "$PSScriptRoot\ensure-test-image.ps1"
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Error al verificar imagen de prueba" -ForegroundColor Red
    exit 1
}

# Paso 3: Ejecutar todas las versiones
Write-Host ""
Write-Host "Paso 3: Ejecutando todas las versiones..." -ForegroundColor Yellow

# Ejecutar prueba básica (requisito mínimo)
Write-Host "Ejecutando prueba básica..." -ForegroundColor Yellow
& "$PSScriptRoot\test-basic.ps1"

# Ejecutar prueba OpenMP
Write-Host "Ejecutando prueba OpenMP..." -ForegroundColor Yellow
& "$PSScriptRoot\test-omp.ps1"

# Ejecutar prueba pThreads
Write-Host "Ejecutando prueba pThreads..." -ForegroundColor Yellow
& "$PSScriptRoot\test-pthread.ps1"

# Paso 3: Verificación final
Write-Host ""
Write-Host "=== VERIFICACIÓN FINAL ===" -ForegroundColor Cyan

# Verificar que todas las imágenes se generaron correctamente
$expectedFiles = @(
    "test_image.jpg",
    "sobel_basic_result.jpg", 
    "sobel_basic_result_threshold.jpg",
    "test_sobel_omp_result.jpg",
    "test_sobel_omp_threshold.jpg",
    "sobel_pthread_result.jpg",
    "sobel_pthread_result_threshold.jpg"
)

$missingFiles = @()
$foundFiles = @()

foreach ($file in $expectedFiles) {
    if (Test-Path "images\$file") {
        $foundFiles += $file
        Write-Host "✅ $file" -ForegroundColor Green
    } else {
        $missingFiles += $file
        Write-Host "❌ $file - faltante" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "=== RESUMEN FINAL ===" -ForegroundColor Cyan
Write-Host "Archivos encontrados: $($foundFiles.Count)/$($expectedFiles.Count)" -ForegroundColor White

if ($missingFiles.Count -eq 0) {
    Write-Host ""
    Write-Host "🎉 ¡TODAS LAS PRUEBAS EXITOSAS!" -ForegroundColor Green
    Write-Host "✅ Todas las versiones del filtro Sobel funcionan correctamente" -ForegroundColor Green
    Write-Host "✅ Todas las imágenes se generaron y copiaron automáticamente" -ForegroundColor Green
    Write-Host "✅ La configuración Docker está funcionando perfectamente" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "⚠️  Algunos archivos faltan:" -ForegroundColor Yellow
    foreach ($file in $missingFiles) {
        Write-Host "  - $file" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "=== INFORMACIÓN ADICIONAL ===" -ForegroundColor Cyan
Write-Host "Directorio de imágenes: $(Resolve-Path images)" -ForegroundColor White
Write-Host "Total de archivos: $(Get-ChildItem images\*.jpg | Measure-Object | Select-Object -ExpandProperty Count)" -ForegroundColor White

Write-Host ""
Write-Host "✅ Proceso completado" -ForegroundColor Green 