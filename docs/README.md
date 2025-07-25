# Filtro Sobel - Prueba Técnica Photonicsens

Este proyecto implementa un filtro Sobel para la detección de bordes en imágenes, desarrollado como parte de una prueba técnica para el puesto de Software Developer en Photonicsens.

## Características

- **Filtro Sobel implementado manualmente**: No utiliza las funciones de OpenCV para el filtro, solo para lectura/escritura de imágenes
- **Detección de bordes**: Aplica los kernels Sobel X e Y para calcular gradientes
- **Umbralización opcional**: Permite obtener bordes más definidos aplicando un umbral
- **Soporte para imágenes color y escala de grises**: Conversión automática a escala de grises
- **Visualización**: Muestra las imágenes original, procesada y con umbral

## Requisitos

- **OpenCV 4.x**: Para lectura y escritura de imágenes
- **CMake 3.10+**: Para la configuración del proyecto
- **Compilador C++**: Compatible con C++17 (GCC, Clang, MSVC)

## Instalación de Dependencias

### Windows (con vcpkg)
```bash
# Instalar vcpkg si no lo tienes
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# Instalar OpenCV
./vcpkg install opencv4:x64-windows
./vcpkg integrate install
```

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install libopencv-dev cmake build-essential
```

### macOS (con Homebrew)
```bash
brew install opencv cmake
```

## Compilación

### Opción 1: Docker (Recomendado)
El proyecto incluye un entorno Docker completo con todas las dependencias preinstaladas.

```powershell
# En Windows PowerShell
.\docker-build.ps1

# O manualmente:
docker-compose build
docker-compose run --rm sobel-dev ./compile.sh
```

### Opción 2: Compilación local
Requiere instalar CMake y OpenCV localmente.

1. **Crear directorio de build**:
```bash
mkdir build
cd build
```

2. **Configurar con CMake**:
```bash
cmake ..
```

3. **Compilar**:
```bash
cmake --build . --config Release
```

## Uso

### Prueba automática
Para verificar que todo funciona correctamente:

#### Con Docker (Recomendado):
```powershell
# En Windows PowerShell
.\docker-build.ps1    # Construir entorno Docker
docker-compose run --rm sobel-dev ./test.sh  # Ejecutar prueba
```

#### Compilación local:
```powershell
# En Windows PowerShell
.\build.ps1    # Compilar
.\test.ps1     # Ejecutar prueba
```

### Uso manual
```bash
./sobel_filter <imagen_entrada> <imagen_salida>
```

### Ejemplos
```bash
# Procesar una imagen
./sobel_filter input.jpg output.jpg

# Procesar imagen PNG
./sobel_filter test.png result.png

# Ejecutar prueba automática
./test_sobel
```

### Salidas
El programa genera dos archivos:
- `output.jpg`: Imagen con el filtro Sobel aplicado
- `output_threshold.jpg`: Imagen con umbral aplicado para bordes más definidos

### Archivos de prueba
El programa de prueba genera:
- `test_image.jpg`: Imagen de prueba con formas geométricas
- `test_sobel_result.jpg`: Resultado del filtro Sobel
- `test_sobel_threshold.jpg`: Resultado con umbral aplicado

## Estructura del Código

### Clase SobelFilter
- **Kernels predefinidos**: Matrices 3x3 para gradientes X e Y
- **applySobel()**: Aplica el filtro Sobel básico
- **applySobelWithThreshold()**: Aplica filtro con umbralización

### Algoritmo
1. **Conversión a escala de grises** (si es necesario)
2. **Aplicación de kernels**: Convolución con matrices Sobel
3. **Cálculo de magnitud**: `sqrt(gx² + gy²)`
4. **Normalización**: Valores entre 0-255
5. **Umbralización opcional**: Para bordes más definidos

## Calidad del Código

### Diseño
- **Orientado a objetos**: Clase encapsulada para el filtro
- **Separación de responsabilidades**: Lógica de filtro separada de I/O
- **Reutilizable**: Fácil extensión para otros filtros

### Rendimiento
- **Acceso eficiente a píxeles**: Uso directo de `cv::Mat::at<>()`
- **Evita copias innecesarias**: Referencias constantes donde es posible
- **Optimización de bucles**: Acceso secuencial a memoria

### Legibilidad
- **Nombres descriptivos**: Variables y funciones claras
- **Comentarios explicativos**: Documentación del algoritmo
- **Estructura clara**: Separación lógica de funcionalidades

### Mantenibilidad
- **Modular**: Fácil modificación de kernels o parámetros
- **Configurable**: Umbral ajustable
- **Extensible**: Base para implementar otros filtros

## Próximos Pasos (Requisitos Deseables)

1. **Programación multihilo**: Implementar OpenMP para paralelización
2. **Optimizaciones SIMD**: Uso de instrucciones vectoriales
3. **Interfaz Android**: Aplicación móvil con NDK y JNI

## Licencia

Este código es parte de una prueba técnica y está destinado únicamente para evaluación. 