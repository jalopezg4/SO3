# GSEA — Gestión Segura y Eficiente de Archivos

**Versión:** 1.0 (Prototipo Educativo)  
**Autor:** Equipo ProyectoFinal — Curso Sistemas Operativos  
**Fecha:** Octubre 2025

---

## Resumen

**GSEA** (Gestión Segura y Eficiente de Archivos) es una utilidad de línea de comandos desarrollada en C++17 que permite **comprimir/descomprimir** y **encriptar/desencriptar** archivos o directorios completos de forma **concurrente**. 

### Características principales:
- Uso directo de **llamadas al sistema POSIX** (open, read, write, opendir, readdir)
- **Procesamiento paralelo** mediante hilos para múltiples archivos
- Implementación **propia** de algoritmos de compresión y cifrado (sin librerías externas)
- Soporte para procesamiento recursivo de directorios
- Control de concurrencia con limitador de hilos

> **Advertencia Académica:** Los algoritmos incluidos (RLE y XOR) son implementaciones educativas con limitaciones conocidas. RLE puede expandir datos poco repetitivos y XOR no es criptográficamente seguro. En entornos de producción se deben usar algoritmos robustos (ej: zlib, AES).

---

## Requisitos del Sistema

### Obligatorios:
- **Sistema Operativo:** Linux (o WSL en Windows)
- **Compilador:** g++ con soporte C++17 o superior
- **Build System:** GNU Make

### Herramientas Opcionales (para pruebas):
- `sha256sum` - Verificación de integridad
- `strace` - Análisis de llamadas al sistema
- `valgrind` - Detección de fugas de memoria
- `time` - Medición de rendimiento
- `diff` / `cmp` - Comparación de archivos

---

## 📁 Estructura del Repositorio

```
SO3/
├── src/
│   ├── main.cpp              # Punto de entrada, parsing de argumentos
│   ├── file_manager.cpp      # I/O con llamadas POSIX (open, read, write)
│   ├── rle.cpp               # Algoritmo de compresión RLE
│   ├── xor_cipher.cpp        # Algoritmo de cifrado XOR
│   ├── worker.cpp            # Lógica de procesamiento por archivo
│   └── thread_limiter.cpp    # Control de concurrencia
├── include/
│   ├── file_manager.h
│   ├── rle.h
│   ├── xor_cipher.h
│   ├── worker.h
│   └── thread_limiter.h
├── obj/                      # Archivos objeto (generado por make)
├── bin/                      # Ejecutable gsea (generado por make)
├── test/                     # Casos de prueba
│   ├── input/
│   └── output/
├── Makefile
└── README.md
```

---

## Compilación

### Compilación estándar (recomendada):

```bash
make
```

Esto generará:
- Archivos objeto en `obj/`
- Ejecutable `bin/gsea`

### Limpiar artefactos de compilación:

```bash
make clean
```

### Compilación manual (alternativa):

```bash
g++ -std=c++17 -O2 -pthread -Iinclude src/*.cpp -o bin/gsea
```

---

## Guía de Uso

### Sintaxis General

```bash
./bin/gsea [operaciones] [opciones] -i <input> -o <output>
```

### Operaciones (pueden combinarse)

Las operaciones se aplican **en el orden especificado**:

| Bandera | Operación | Descripción |
|---------|-----------|-------------|
| `-c` | Comprimir | Aplica algoritmo de compresión |
| `-d` | Descomprimir | Revierte compresión |
| `-e` | Encriptar | Aplica algoritmo de cifrado |
| `-u` | Desencriptar | Revierte cifrado |

**Ejemplo:** `-c -e` primero comprime, luego encripta

### Opciones

| Opción | Parámetro | Descripción | Default |
|--------|-----------|-------------|---------|
| `--comp-alg` | `<algoritmo>` | Algoritmo de compresión | `rle` |
| `--enc-alg` | `<algoritmo>` | Algoritmo de encriptación | `xor` |
| `-i` | `<ruta>` | Archivo o directorio de entrada | **Requerido** |
| `-o` | `<ruta>` | Archivo o directorio de salida | **Requerido** |
| `-k` | `<clave>` | Clave secreta para cifrado | Opcional |
| `--max-threads` | `<N>` | Máximo de hilos concurrentes | CPU cores |

---

## Ejemplos Prácticos

### 1. Comprimir un archivo con RLE

```bash
./bin/gsea -c --comp-alg rle -i datos/archivo.bin -o salida/archivo.rle
```

### 2. Descomprimir un archivo RLE

```bash
./bin/gsea -d --comp-alg rle -i salida/archivo.rle -o restaurado/archivo.bin
```

### 3. Encriptar un archivo (XOR con clave)

```bash
./bin/gsea -e --enc-alg xor -i documentos/secret.txt -o documentos/secret.txt.enc -k "MiClaveSecreta"
```

### 4. Desencriptar (misma clave)

```bash
./bin/gsea -u --enc-alg xor -i documentos/secret.txt.enc -o documentos/secret_restored.txt -k "MiClaveSecreta"
```

### 5. Comprimir y encriptar un directorio completo (concurrente)

```bash
./bin/gsea -c -e --comp-alg rle --enc-alg xor \
  -i ./Resultados/2025-10-15 \
  -o ./Archivados/2025-10-15.bak \
  -k "G3n0m3S3cur1ty!" \
  --max-threads 8
```

### 6. Restaurar directorio (orden invertido: desencriptar → descomprimir)

```bash
./bin/gsea -u -d --enc-alg xor --comp-alg rle \
  -i ./Archivados/2025-10-15.bak \
  -o ./Restaurados/2025-10-15 \
  -k "G3n0m3S3cur1ty!" \
  --max-threads 8
```

---

## Comportamiento y Detalles Funcionales

### Procesamiento de Archivos

- **Archivo → Archivo:** Procesa el archivo único especificado
- **Directorio → Directorio:** 
  - Recorre recursivamente todos los archivos
  - Preserva la estructura de directorios relativa
  - Crea directorios de salida automáticamente

### Concurrencia

- Un **hilo independiente** por cada archivo procesado
- Control de concurrencia mediante `ThreadLimiter` (mutex + condition_variable)
- Límite configurable con `--max-threads` para evitar sobrecarga del sistema

### I/O de Bajo Nivel

Utiliza **exclusivamente llamadas al sistema POSIX**:
- `open()`, `read()`, `write()`, `close()` - Manipulación de archivos
- `opendir()`, `readdir()`, `closedir()` - Navegación de directorios
- `stat()`, `mkdir()` - Manejo de metadatos y estructura

**NO** usa funciones de alto nivel como `fopen()`, `fread()`, `fwrite()`

### Gestión de Memoria

- Los archivos se cargan **completamente en memoria** antes de procesarse
- Para archivos grandes (>500MB) se recomienda refactorizar a procesamiento por bloques (streaming)

---

## Algoritmos Implementados

### 1. RLE (Run-Length Encoding) - Compresión

**Implementación:** Codificación byte-wise simple

```
Formato: [count][value] [count][value] ...
- count: 1-255 (número de repeticiones)
- value: byte a repetir
```

**Ventajas:**
- Complejidad temporal O(n)
- Extremadamente simple y rápido
- Excelente para datos con **muchas repeticiones** consecutivas
- Ideal para: imágenes monocromáticas, archivos de log, datos de secuenciación

**Limitaciones:**
- Puede **expandir** datos poco repetitivos (peor caso: 2x tamaño original)
- No tan eficiente como LZW o Huffman para datos generales
- No detecta patrones complejos

**Caso de uso óptimo:** Datos con secuencias largas de bytes idénticos

### 2. XOR Cipher (Cifrado Vigenère sobre bytes) - Encriptación

**Implementación:** XOR con clave repetida cíclicamente

```c++
out[i] = in[i] ^ key[i % keylen]
```

**Ventajas:**
- Implementación trivial (1 línea)
- Operación reversible: cifrar = descifrar
- Sin overhead computacional
- Didácticamente útil para entender cifrado de flujo

**Limitaciones:**
- **NO es criptográficamente seguro**
- Vulnerable a análisis de frecuencia
- Vulnerable a ataques de texto conocido (known-plaintext)
- **NO usar en producción**

**Propósito:** Exclusivamente educativo para demostrar conceptos básicos de cifrado

---

## Suite de Pruebas Recomendadas

### 1. Prueba Básica (Integridad)

```bash
# Crear archivo de prueba
echo "Hola Mundo" > test/input/hello.txt

# Encriptar
./bin/gsea -e -k "clave123" -i test/input/hello.txt -o test/output/hello.enc

# Desencriptar
./bin/gsea -u -k "clave123" -i test/output/hello.enc -o test/output/hello_dec.txt

# Verificar integridad
diff test/input/hello.txt test/output/hello_dec.txt
```

### 2. Prueba de Compresión (RLE Efectivo)

```bash
# Generar archivo con repeticiones (10KB de 'A' + 5KB de null bytes)
python3 -c "open('test/input/repetitive.bin','wb').write(b'A'*10000 + b'\x00'*5000)"

# Comprimir
./bin/gsea -c -i test/input/repetitive.bin -o test/output/repetitive.rle

# Verificar reducción de tamaño
ls -lh test/input/repetitive.bin test/output/repetitive.rle

# Descomprimir y verificar
./bin/gsea -d -i test/output/repetitive.rle -o test/output/repetitive_restored.bin
cmp test/input/repetitive.bin test/output/repetitive_restored.bin && echo "✓ Integridad OK"
```

### 3. Prueba de Concurrencia (Benchmark)

```bash
# Crear múltiples archivos
mkdir -p test/multi_input
for i in {1..20}; do 
    dd if=/dev/urandom of=test/multi_input/file$i.dat bs=1M count=1 2>/dev/null
done

# Procesar con 1 hilo
time ./bin/gsea -c -i test/multi_input -o test/multi_output_1t --max-threads 1

# Procesar con N hilos
time ./bin/gsea -c -i test/multi_input -o test/multi_output_Nt --max-threads 8
```

### 4. Verificación de Llamadas POSIX

```bash
# Rastrear syscalls con strace
strace -f -e trace=open,read,write,opendir,readdir \
  ./bin/gsea -c -i test/input/a.txt -o test/output/a.rle 2>&1 | grep -E "open|read|write"
```

### 5. Detección de Fugas de Memoria

```bash
valgrind --leak-check=full --show-leak-kinds=all \
  ./bin/gsea -c -i test/input/a.txt -o test/output/a.rle
```

### 6. Integridad con Checksums

```bash
# Checksum original
sha256sum test/input/original.dat > checksum_original.txt

# Comprimir + Encriptar
./bin/gsea -c -e -k "test" -i test/input/original.dat -o test/output/processed.dat

# Desencriptar + Descomprimir
./bin/gsea -u -d -k "test" -i test/output/processed.dat -o test/output/restored.dat

# Verificar checksum
sha256sum test/output/restored.dat > checksum_restored.txt
diff checksum_original.txt checksum_restored.txt && echo "✓ Integridad Perfecta"
```

---

## Solución de Problemas Comunes

### Error: `No rule to make target 'main.cpp'`
**Causa:** Makefile no encuentra los archivos fuente  
**Solución:** Verificar que `src/main.cpp` existe y que el Makefile apunta a `src/*.cpp`

### Error: `undefined reference to 'xor_crypt'`
**Causa:** Falta implementación o no se está enlazando correctamente  
**Solución:** Verificar que `src/xor_cipher.cpp` contiene la implementación completa

### Error: `Permission denied`
**Causa:** Sin permisos de escritura en el directorio de salida  
**Solución:** Usar rutas donde tengas permisos o ejecutar `chmod +w <directorio>`

### Error: `Output must be directory`
**Causa:** La entrada es un directorio pero la salida no  
**Solución:** Si `-i` es directorio, `-o` debe ser directorio (terminar con `/` o no existir)

### Archivo comprimido es más grande que el original
**Causa:** RLE expande datos poco repetitivos  
**Solución:** Normal para este algoritmo. RLE solo es eficiente con datos muy repetitivos

---

## Mejoras Futuras (Roadmap)

### Corto Plazo:
- [ ] Implementar cabecera mágica con metadata (algoritmo, versión, flags)
- [ ] Añadir validación de integridad (CRC32/SHA256)
- [ ] Modo verbose con estadísticas de compresión
- [ ] Barra de progreso para operaciones largas

### Mediano Plazo:
- [ ] Implementar **LZW** o **Huffman** para mejor compresión general
- [ ] Implementar **Vigenère** o **DES simplificado** para cifrado más robusto
- [ ] Procesamiento por bloques (streaming) para archivos grandes
- [ ] Preservar metadatos (permisos, timestamps) con `stat()` / `utimensat()`

### Largo Plazo:
- [ ] Suite completa de pruebas unitarias (Google Test)
- [ ] Scripts de benchmarking automático
- [ ] Soporte multi-plataforma (Windows API)
- [ ] Compresión adaptativa (selección automática de algoritmo)

---

## Licencia

```
MIT License

Copyright (c) 2025 Equipo ProyectoFinal - Sistemas Operativos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Soporte y Contribuciones

### Reportar Issues
Si encuentras errores al compilar o ejecutar, por favor incluye:
- Salida completa de `make` o del comando fallido
- Versión de g++ (`g++ --version`)
- Sistema operativo y distribución

### Contribuciones
Para extender funcionalidad (ej: implementar LZW, streaming, etc.), seguir la estructura modular existente:
1. Crear header en `include/`
2. Implementar en `src/`
3. Integrar en `worker.cpp`
4. Actualizar `Makefile` si es necesario

---

## Contexto Académico

Este proyecto fue desarrollado como parte del **Examen Final Práctico** del curso de Sistemas Operativos, cumpliendo los siguientes objetivos de aprendizaje:

1. **Gestión de archivos mediante llamadas al sistema** (no abstracciones de librería)
2. **Implementación de concurrencia** para optimización de rendimiento
3. **Investigación e implementación** de algoritmos de compresión y cifrado desde cero
4. **Desarrollo de software de bajo nivel** en C/C++