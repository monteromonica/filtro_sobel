# Script PowerShell para Filtro Sobel
# Alternativa al Makefile para Windows

param(
    [string]$Action = "help",
    [string]$Input = "",
    [string]$Output = ""
)

function Show-Help {
    Write-Host "=== FILTRO SOBEL - COMANDOS DISPONIBLES ===" -ForegroundColor Green
    Write-Host ""
    Write-Host "Comandos principales:" -ForegroundColor Yellow
    Write-Host "  .\build.ps1 build          - Compilar el proyecto"
    Write-Host "  .\build.ps1 run-test       - Ejecutar con imagen de prueba"
    Write-Host "  .\build.ps1 run -Input mi_imagen.jpg -Output resultado.jpg"
    Write-Host "  .\build.ps1 up              - Levantar Docker"
    Write-Host "  .\build.ps1 down            - Bajar Docker"
    Write-Host "  .\build.ps1 clean-build     - Limpiar build"
    Write-Host ""
    Write-Host "Ejemplo:" -ForegroundColor Cyan
    Write-Host "  .\build.ps1 run -Input mi_foto.jpg -Output bordes.jpg"
}

function Start-Docker {
    Write-Host "Levantando Docker..." -ForegroundColor Green
    docker-compose -f docker/docker-compose.fast.yml up -d
}

function Stop-Docker {
    Write-Host "Bajando Docker..." -ForegroundColor Green
    docker-compose -f docker/docker-compose.fast.yml down
}

function Build-Project {
    Write-Host "Compilando proyecto..." -ForegroundColor Green
    docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace && mkdir -p build && cd build && cmake .. && make"
}

function Run-Test {
    Write-Host "Ejecutando con imagen de prueba..." -ForegroundColor Green
    docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && ./sobel_filter /workspace/images/test_image.jpg resultado_test.jpg"
}

function Run-Custom {
    if ([string]::IsNullOrEmpty($Input) -or [string]::IsNullOrEmpty($Output)) {
        Write-Host "Error: Debes especificar Input y Output" -ForegroundColor Red
        Show-Help
        return
    }
    Write-Host "Ejecutando con imagen personalizada..." -ForegroundColor Green
    docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && ./sobel_filter /workspace/images/$Input $Output"
}

function Clean-Build {
    Write-Host "Limpiando build..." -ForegroundColor Green
    docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "rm -rf /workspace/build"
}

# Ejecutar acción
switch ($Action.ToLower()) {
    "help" { Show-Help }
    "up" { Start-Docker }
    "down" { Stop-Docker }
    "build" { Start-Docker; Build-Project }
    "run-test" { Start-Docker; Build-Project; Run-Test }
    "run" { Start-Docker; Build-Project; Run-Custom }
    "clean-build" { Clean-Build }
    default { 
        Write-Host "Acción '$Action' no reconocida" -ForegroundColor Red
        Show-Help 
    }
} 