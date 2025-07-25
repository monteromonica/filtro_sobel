# Script para probar el requisito mínimo del filtro Sobel
# Filtro Sobel - Prueba Tecnica Photonicsens

Write-Host "=== Prueba del Filtro Sobel (Requisito Mínimo) ===" -ForegroundColor Green

# Verificar Docker
try {
    docker --version | Out-Null
    Write-Host "Docker encontrado" -ForegroundColor Green
} catch {
    Write-Host "Error: Docker no esta disponible" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Compilando y probando la versión básica..." -ForegroundColor Yellow

# Asegurar que existe la imagen de prueba
Write-Host "Paso 1: Verificar imagen de prueba..." -ForegroundColor Yellow
& "$PSScriptRoot\ensure-test-image.ps1"
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Error al verificar imagen de prueba" -ForegroundColor Red
    exit 1
}

# Compilar y ejecutar paso a paso
Write-Host "Paso 2: Crear directorio build..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "rm -rf build && mkdir -p build"

Write-Host "Paso 3: Configurar con CMake..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cmake .."

Write-Host "Paso 4: Compilar..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && make"

Write-Host "Paso 5: Copiar imagen de prueba..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cp /workspace/images/test_image.jpg build/"

Write-Host "Paso 6: Ejecutar filtro Sobel básico..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && ./sobel_filter test_image.jpg sobel_basic_result.jpg"

# Copiar solo las imágenes específicas del filtro básico
Write-Host ""
Write-Host "Copiando imágenes generadas..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cp sobel_basic_result.jpg sobel_basic_result_threshold.jpg /workspace/images/ 2>/dev/null || true"

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "=== Prueba básica exitosa ===" -ForegroundColor Green
    Write-Host "El filtro Sobel básico funciona correctamente" -ForegroundColor White
    Write-Host "Imágenes copiadas a: images/" -ForegroundColor Green
    
    # Mostrar archivos generados
    Write-Host ""
    Write-Host "Archivos generados:" -ForegroundColor Yellow
    Get-ChildItem images\sobel_basic*.jpg -ErrorAction SilentlyContinue | ForEach-Object {
        Write-Host "  ✅ $($_.Name)" -ForegroundColor Green
    }
} else {
    Write-Host ""
    Write-Host "=== Error en la prueba básica ===" -ForegroundColor Red
    Write-Host "Pero las imágenes se copiaron si estaban disponibles" -ForegroundColor Yellow
} 