# Filtro Sobel - Prueba Técnica Photonicsens

Implementación de un filtro Sobel para detección de bordes en imágenes, desarrollado como parte de una prueba técnica para el puesto de Software Developer.

## 📁 Estructura del Proyecto

```
filtro-sobel/
├── src/                    # Código fuente principal
│   ├── sobel_filter.cpp    # Implementación básica del filtro Sobel
│   ├── sobel_filter_omp.cpp # Versión optimizada con OpenMP
│   └── sobel_filter_pthread.cpp # Versión con pThreads
├── tests/                  # Programas de prueba
│   ├── test_sobel.cpp      # Prueba con interfaz gráfica
│   ├── test_sobel_no_gui.cpp # Prueba sin GUI (Docker)
│   └── test_sobel_omp.cpp  # Prueba específica para OpenMP
├── scripts/                # Scripts de automatización
│   ├── ensure-test-image.ps1 # Verifica/genera imagen de prueba
│   ├── test-omp.ps1        # Prueba específica para OpenMP
│   ├── test-pthread.ps1    # Prueba específica para pThreads
│   ├── clean-images.ps1    # Limpieza de imágenes
│   ├── run-complete-test.ps1 # Script maestro completo
│   └── compare-performance.ps1 # Compara rendimiento
├── docker/                 # Configuración Docker
│   ├── Dockerfile.fast     # Imagen Docker rápida
│   ├── docker-compose.fast.yml
│   └── .dockerignore
├── docs/                   # Documentación
│   ├── README.md           # Documentación detallada
│   └── dependencies.txt    # Dependencias del proyecto
├── images/                 # Imágenes de entrada/salida
│   ├── test_image.jpg      # Imagen de prueba única (10KB)
│   ├── sobel_basic_*.jpg   # Resultados del filtro básico
│   ├── sobel_improved_*.jpg # Resultados del filtro mejorado
│   ├── test_sobel_omp_*.jpg # Resultados del filtro OpenMP
│   └── sobel_pthread_*.jpg # Resultados del filtro pThreads
├── build/                  # Directorio de compilación
├── CMakeLists.txt          # Configuración CMake
└── .gitignore             # Archivos a ignorar
```

## 🚀 Inicio Rápido

### Opción 1: Uso Individual (Recomendado para pruebas específicas)

#### Windows (PowerShell)
```powershell
# 1. Levantar Docker
docker-compose -f docker/docker-compose.fast.yml up -d

# 2. Compilar el proyecto
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace && mkdir -p build && cd build && cmake .. && make"

# 3. Ejecutar con imagen específica
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && ./sobel_filter /workspace/images/mi_imagen.jpg resultado.jpg"

# 4. Copiar resultados a tu carpeta local
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && cp resultado.jpg resultado_threshold.jpg /workspace/images/ 2>/dev/null || echo 'Imágenes no encontradas'"
```

#### Linux/macOS (Bash)
```bash
# 1. Levantar Docker
docker-compose -f docker/docker-compose.fast.yml up -d

# 2. Compilar el proyecto
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace && mkdir -p build && cd build && cmake .. && make"

# 3. Ejecutar con imagen específica
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && ./sobel_filter /workspace/images/mi_imagen.jpg resultado.jpg"

# 4. Copiar resultados a tu carpeta local
docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && cp resultado.jpg resultado_threshold.jpg /workspace/images/ 2>/dev/null || echo 'Imágenes no encontradas'"
```

### Opción 2: Scripts Automatizados

#### Scripts Bash (Portables - Windows/Linux/macOS)
```bash
# Prueba básica (requisito mínimo)
bash scripts/test-basic.sh

# Prueba OpenMP
bash scripts/test-omp.sh

# Prueba pThreads
bash scripts/test-pthread.sh

# Prueba versión mejorada
bash scripts/test-improved.sh

# Ejecutar todas las pruebas
bash scripts/run-complete-test.sh

# Comparar rendimiento
bash scripts/compare-performance.sh
```

#### Scripts PowerShell (Solo Windows)
```powershell
# Prueba básica (requisito mínimo)
.\scripts\test-basic.ps1

# Prueba OpenMP
.\scripts\test-omp.ps1

# Prueba pThreads
.\scripts\test-pthread.ps1

# Ejecutar todas las pruebas
.\scripts\run-complete-test.ps1

# Comparar rendimiento
.\scripts\compare-performance.ps1
```

### Opción 3: Scripts Simplificados (build.ps1/build.sh)

#### Windows (PowerShell)
```powershell
# Ver ayuda
.\build.ps1

# Compilar y ejecutar con imagen de prueba
.\build.ps1 run-test

# Ejecutar con tu imagen
.\build.ps1 run -Input mi_foto.jpg -Output bordes.jpg
```

#### Linux/macOS (Bash)
```bash
# Ver ayuda
./build.sh

# Compilar y ejecutar con imagen de prueba
./build.sh run-test

# Ejecutar con tu imagen
./build.sh run INPUT=mi_foto.jpg OUTPUT=bordes.jpg
```

### Opción 4: Usando Makefile (Recomendado para usuarios avanzados)
```bash
# Ver comandos disponibles
make help

# Ejecutar todas las pruebas
make test-all

# Prueba específica
make test-basic
make test-omp
make test-pthread
```

## 📸 Uso con Imágenes Propias

### Paso a Paso:

1. **Coloca tu imagen en la carpeta `images/`**
   ```bash
   # Ejemplo: copia tu imagen
   cp mi_foto.jpg images/
   ```

2. **Ejecuta el filtro Sobel**
   ```bash
   # Windows (PowerShell)
   docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && ./sobel_filter /workspace/images/mi_foto.jpg resultado.jpg"
   
   # Linux/macOS (Bash)
   docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && ./sobel_filter /workspace/images/mi_foto.jpg resultado.jpg"
   ```

3. **Copia los resultados a tu carpeta local**
   ```bash
   # Windows (PowerShell)
   docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && cp resultado.jpg resultado_threshold.jpg /workspace/images/"
   
   # Linux/macOS (Bash)
   docker-compose -f docker/docker-compose.fast.yml run --rm sobel-dev-fast bash -c "cd /workspace/build && cp resultado.jpg resultado_threshold.jpg /workspace/images/"
   ```

### Resultados Generados:
- `resultado.jpg` - Gradientes Sobel (escala de grises)
- `resultado_threshold.jpg` - Bordes detectados (blanco y negro)

### Nota sobre el Ruido:
El filtro Sobel detecta todos los cambios de intensidad, incluyendo texturas finas. Esto es normal y esperado del algoritmo. Para bordes más limpios se requerirían algoritmos más avanzados como Canny.

## 📋 Resumen de Scripts Disponibles

### Scripts Principales:
- `build.ps1` / `build.sh` - Scripts simplificados para uso individual
- `Makefile` - Comandos avanzados (requiere make instalado)

### Scripts de Testing (scripts/):
- `test-basic.sh/.ps1` - Prueba del requisito mínimo
- `test-omp.sh/.ps1` - Prueba versión OpenMP
- `test-pthread.sh/.ps1` - Prueba versión pThreads
- `test-improved.sh` - Prueba versión mejorada
- `run-complete-test.sh/.ps1` - Ejecuta todas las pruebas
- `compare-performance.sh/.ps1` - Compara rendimiento

### Scripts de Utilidad:
- `clean-images.sh/.ps1` - Limpia imágenes generadas
- `ensure-test-image.sh/.ps1` - Verifica/genera imagen de prueba
make test-all

# Pruebas individuales
make test-basic
make test-omp
make test-pthread
make test-improved

# Comparar rendimiento
make compare

# Limpiar imágenes
make clean
```

### Opción 4: Compilación Local
```bash
# Windows/Linux/macOS
mkdir build && cd build
cmake ..
make
```

## 📋 Características

- ✅ **Filtro Sobel implementado manualmente** (sin usar OpenCV para el filtro)
- ✅ **Soporte para imágenes color y escala de grises**
- ✅ **Umbralización configurable** para bordes más definidos
- ✅ **Programación Multihilo**:
  - **OpenMP**: Paralelización automática con directivas `#pragma`
  - **pThreads**: Control manual de hilos con división de trabajo
  - **Comparación de rendimiento**: Speedup y eficiencia medidos
- ✅ **C++ Moderno**:
  - **Excepciones personalizadas**: Manejo robusto de errores
  - **std::optional**: Valores opcionales para resultados
  - **constexpr**: Constantes en tiempo de compilación
  - **Move semantics**: Optimización de rendimiento
  - **Documentación Doxygen**: Comentarios profesionales
- ✅ **Entorno Docker completo** con todas las dependencias
- ✅ **Scripts de automatización portables** (Bash para Windows/Linux/macOS + PowerShell para Windows)
- ✅ **Estructura profesional** y bien organizada

## 🔧 Requisitos

- **Docker Desktop** (para entorno Docker)
- **CMake 3.10+** (para compilación local)
- **OpenCV 4.0+** (para compilación local)
- **Compilador C++17** (GCC, Clang, MSVC)

## 📖 Documentación

Para información detallada, consulta:
- [Documentación completa](docs/README.md)
- [Dependencias del proyecto](docs/dependencies.txt)

## 🧪 Pruebas

El proyecto incluye:
- **Prueba automática**: Genera imágenes de prueba y aplica el filtro
- **Visualización**: Muestra resultados en ventanas (versión con GUI)
- **Estadísticas**: Información detallada sobre los resultados

## 🎯 Estado del Proyecto

### ✅ Completado
1. **Requisito mínimo**: Filtro Sobel implementado y funcionando
2. **Requisito deseable**: Programación multihilo implementada
   - **OpenMP**: Speedup de ~0.95x en 16 hilos (overhead para imágenes pequeñas)
   - **pThreads**: Speedup de ~3.50x en 16 hilos (mejor rendimiento)
   - **Comparación justa**: Todas las versiones usan la misma imagen de entrada (10KB)
   - **Resultados consistentes**: Tamaños de archivo similares entre versiones (~12KB)

### 🔄 Próximos Pasos
1. **Requisito opcional**: Interfaz Android con NDK y JNI

## 📄 Licencia

Este código es parte de una prueba técnica y está destinado únicamente para evaluación. 

## 📱 Requisito Opcional: Android NDK/JNI

> **Nota:**
>
> Mi experiencia principal es como desarrolladora C++/Python, no en desarrollo Android. Sin embargo, he preparado el código nativo siguiendo las mejores prácticas para su integración con Android NDK/JNI:
>
> - El archivo `android/sobel_jni.cpp` expone la funcionalidad de los filtros mediante JNI, usando los patrones Strategy y Factory.
> - El archivo `android/CMakeLists.txt` permite compilar la librería nativa (`libsobel.so`) para Android.
> - He documentado cómo se integraría en un proyecto Android Studio y cómo se llamaría desde Java/Kotlin.
>
>
> **¿Qué entrego?**
>
> - Código C++ preparado para Android NDK/JNI
> - Ejemplo de clase Java/Kotlin para llamar a la librería nativa
> - Instrucciones claras para la integración en Android Studio
>

## 🏗️ Arquitectura y Justificación

> **Nota:**
>
> El proyecto está estructurado para mostrar la evolución profesional del código y facilitar la comparación de rendimiento y calidad entre versiones. Cada versión cumple un objetivo específico del enunciado:
>
> - **sobel_filter.cpp**: Versión básica, secuencial, cumple el requisito mínimo.
> - **sobel_filter_improved.cpp**: Versión secuencial profesional, con C++ moderno, manejo robusto de errores y arquitectura preparada para patrones de diseño y Android NDK. **No incluye multihilo a propósito** para mantener la claridad y la comparación directa con las versiones paralelas.
> - **sobel_filter_omp.cpp**: Versión multihilo usando OpenMP, optimizada solo para rendimiento. Mantiene la lógica separada para facilitar la comparación y la extensibilidad.
> - **sobel_filter_pthread.cpp**: Versión multihilo usando pThreads, optimizada solo para rendimiento. Mantiene la lógica separada por los mismos motivos.
>
>
> **Patrones de diseño implementados:**
>
> - **Strategy Pattern**: Permite elegir el algoritmo de detección de bordes en tiempo de ejecución (secuencial, OpenMP, pThreads, etc.).
> - **Factory Pattern**: Centraliza la creación de filtros y facilita la extensibilidad.
>
> **Recomendación para la revisión:**
>
> - Revise la versión improved para ver buenas prácticas de C++ moderno y arquitectura profesional.
> - Revise las versiones OMP y pThreads para ver la implementación multihilo y la comparación de rendimiento.
> - Revise los patrones de diseño para ver la extensibilidad y la preparación para Android NDK/JNI.

--- 