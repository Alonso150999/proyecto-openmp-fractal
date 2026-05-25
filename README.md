# Proyecto OpenMP: Mandelbrot y Convolución 2D

## Descripción

Este proyecto implementa una aplicación en C++ para Linux que genera una imagen del conjunto de Mandelbrot y aplica un filtro de convolución 2D pesado. Posteriormente, se optimiza usando OpenMP para analizar paralelismo, balanceo de carga, sincronización, false sharing, vectorización SIMD y afinidad de hilos.

## Entorno de prueba

- Sistema operativo: Ubuntu 24.04 en VMware
- CPU: AMD Ryzen 7 7840HS w/ Radeon 780M Graphics
- vCPU asignadas: 4
- Configuración VMware: 2 procesadores x 2 núcleos
- RAM asignada: 4 GB
- Compilador: g++ 13.3.0
- Caché L1d: 128 KiB
- Caché L1i: 128 KiB
- Caché L2: 4 MiB
- Caché L3: 32 MiB

## Resolución usada

Debido a que el entorno de prueba es una máquina virtual con 4 GB de RAM, se utilizó una resolución de 4096x4096 en lugar de 8K completo, manteniendo una carga computacional alta mediante 2000 iteraciones de Mandelbrot y un filtro Gaussiano 15x15.

## Estructura del repositorio

```text
src/
  01_secuencial.cpp
  02_openmp_base_ia.cpp
  03_schedulers.cpp
  04_histograma.cpp
  05_vectorizacion.cpp

scripts/
  compilar.sh
  benchmark.sh
  affinity_tests.sh

resultados/
  tiempos.csv
  scheduler_tests.csv
  histograma.csv
  afinidad.csv

output/
  fractal.ppm
  filtrada.ppm

docs/
  prompts.txt
  capturas/
