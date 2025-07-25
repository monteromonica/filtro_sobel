# Script para asegurar que existe la imagen de prueba
# Filtro Sobel - Prueba Tecnica Photonicsens

param(
    [switch]$Force = $false  # Para forzar regeneración
)

Write-Host "=== Verificacion de Imagen de Prueba ===" -ForegroundColor Yellow

# Verificar si la imagen existe
if ((Test-Path "images\test_image.jpg") -and (-not $Force)) {
    $fileInfo = Get-Item "images\test_image.jpg"
    $sizeKB = [math]::Round($fileInfo.Length / 1KB, 1)
    Write-Host "✅ Imagen de prueba encontrada: images\test_image.jpg ($sizeKB KB)" -ForegroundColor Green
    return $true
}

# Si no existe o se fuerza regeneración, crear la imagen
Write-Host "🔄 Generando imagen de prueba..." -ForegroundColor Yellow

# Verificar Docker
try {
    docker --version | Out-Null
} catch {
    Write-Host "❌ Error: Docker no esta disponible" -ForegroundColor Red
    return $false
}

# Crear directorio build limpio
Write-Host "Paso 1: Preparar entorno..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "rm -rf build && mkdir -p build"

# Configurar y compilar
Write-Host "Paso 2: Configurar y compilar..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cmake .. && make"

# Generar imagen de prueba
Write-Host "Paso 3: Generar imagen de prueba..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && ./test_sobel_no_gui"

# Copiar solo la imagen de prueba
Write-Host "Paso 4: Copiar imagen..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cp test_image.jpg /workspace/images/ 2>/dev/null || true"

# Verificar que se copió correctamente
if (Test-Path "images\test_image.jpg") {
    $fileInfo = Get-Item "images\test_image.jpg"
    $sizeKB = [math]::Round($fileInfo.Length / 1KB, 1)
    Write-Host "✅ Imagen de prueba generada exitosamente: images\test_image.jpg ($sizeKB KB)" -ForegroundColor Green
    return $true
} else {
    Write-Host "❌ Error: No se pudo generar la imagen de prueba" -ForegroundColor Red
    return $false
} 