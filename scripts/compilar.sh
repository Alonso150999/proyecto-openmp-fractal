#!/bin/bash

echo "Compilando proyecto OpenMP..."

g++ -O3 src/01_secuencial.cpp -o secuencial

g++ -O3 -fopenmp src/02_openmp_base_ia.cpp -o openmp_base

g++ -O3 -fopenmp src/03_schedulers.cpp -o schedulers

g++ -O3 -fopenmp src/04_histograma.cpp -o histograma

g++ -O3 -march=native -fopenmp -fopt-info-vec-optimized src/05_vectorizacion.cpp -o vectorizacion

echo "Compilacion finalizada."
