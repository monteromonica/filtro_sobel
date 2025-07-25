#!/bin/bash

# Script Bash para Filtro Sobel
# Alternativa al Makefile para Linux/macOS

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Variables por defecto
ACTION="help"
INPUT=""
OUTPUT=""

# Función para mostrar ayuda
show_help() {
    echo -e "${GREEN}=== FILTRO SOBEL - COMANDOS DISPONIBLES ===${NC}"
    echo ""
    echo -e "${YELLOW}Comandos principales:${NC}"
    echo "  ./build.sh build          - Compilar el proyecto"
    echo "  ./build.sh run-test       - Ejecutar con imagen de prueba"
    echo "  ./build.sh run INPUT=mi_imagen.jpg OUTPUT=resultado.jpg"
    echo "  ./build.sh up              - Levantar Docker"
    echo "  ./build.sh down            - Bajar Docker"
    echo "  ./build.sh clean-build     - Limpiar build"
    echo ""
    echo -e "${CYAN}Ejemplo:${NC}"
    echo "  ./build.sh run INPUT=mi_foto.jpg OUTPUT=bordes.jpg"
}

# Función para levantar Docker
start_docker() {
    echo -e "${GREEN}Levantando Docker...${NC}"
    docker-compose -f docker/docker-compose.fast.yml up -d
}

# Función para bajar Docker
stop_docker() {
    echo -e "${GREEN}Bajando Docker...${NC}"
    docker-compose -f docker/docker-compose.fast.yml down
}

# Función para compilar proyecto
build_project() {
    echo -e "${GREEN}Compilando proyecto...${NC}"
    docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace && mkdir -p build && cd build && cmake .. && make"
}

# Función para ejecutar con imagen de prueba
run_test() {
    echo -e "${GREEN}Ejecutando con imagen de prueba...${NC}"
    docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && ./sobel_filter /workspace/images/test_image.jpg resultado_test.jpg"
}

# Función para ejecutar con imagen personalizada
run_custom() {
    if [ -z "$INPUT" ] || [ -z "$OUTPUT" ]; then
        echo -e "${RED}Error: Debes especificar INPUT y OUTPUT${NC}"
        show_help
        exit 1
    fi
    echo -e "${GREEN}Ejecutando con imagen personalizada...${NC}"
    docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && ./sobel_filter /workspace/images/$INPUT $OUTPUT"
}

# Función para limpiar build
clean_build() {
    echo -e "${GREEN}Limpiando build...${NC}"
    docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "rm -rf /workspace/build"
}

# Procesar argumentos
while [[ $# -gt 0 ]]; do
    case $1 in
        build|up|down|run-test|clean-build|help)
            ACTION="$1"
            shift
            ;;
        run)
            ACTION="run"
            shift
            ;;
        INPUT=*)
            INPUT="${1#INPUT=}"
            shift
            ;;
        OUTPUT=*)
            OUTPUT="${1#OUTPUT=}"
            shift
            ;;
        *)
            echo -e "${RED}Argumento no reconocido: $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

# Ejecutar acción
case $ACTION in
    "help")
        show_help
        ;;
    "up")
        start_docker
        ;;
    "down")
        stop_docker
        ;;
    "build")
        start_docker
        build_project
        ;;
    "run-test")
        start_docker
        build_project
        run_test
        ;;
    "run")
        start_docker
        build_project
        run_custom
        ;;
    "clean-build")
        clean_build
        ;;
    *)
        echo -e "${RED}Acción '$ACTION' no reconocida${NC}"
        show_help
        exit 1
        ;;
esac 