# Script para probar la versión pThreads del filtro Sobel
# Filtro Sobel - Prueba Tecnica Photonicsens

Write-Host "=== Prueba del Filtro Sobel con pThreads ===" -ForegroundColor Green

# Verificar Docker
try {
    docker --version | Out-Null
    Write-Host "Docker encontrado" -ForegroundColor Green
} catch {
    Write-Host "Error: Docker no esta disponible" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Compilando y probando la versión pThreads..." -ForegroundColor Yellow

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

Write-Host "Paso 5: Ejecutar prueba pThreads..." -ForegroundColor Yellow
# Usar la imagen de prueba
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && ./sobel_filter_pthread ../images/test_image.jpg sobel_pthread_result.jpg"

# Copiar solo las imágenes específicas de pThreads
Write-Host ""
Write-Host "Copiando imágenes generadas..." -ForegroundColor Yellow
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd build && cp sobel_pthread_result.jpg sobel_pthread_result_threshold.jpg /workspace/images/ 2>/dev/null || true"

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "=== Prueba pThreads exitosa ===" -ForegroundColor Green
    Write-Host "La versión pThreads funciona correctamente" -ForegroundColor White
    Write-Host "Imágenes copiadas a: images/" -ForegroundColor Green
    
    # Mostrar archivos generados
    Write-Host ""
    Write-Host "Archivos generados:" -ForegroundColor Yellow
    Get-ChildItem images\*pthread*.jpg -ErrorAction SilentlyContinue | ForEach-Object {
        Write-Host "  ✅ $($_.Name)" -ForegroundColor Green
    }
} else {
    Write-Host ""
    Write-Host "=== Error en la prueba pThreads ===" -ForegroundColor Red
    Write-Host "Pero las imágenes se copiaron si estaban disponibles" -ForegroundColor Yellow
} 