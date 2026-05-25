#!/bin/bash

echo "Hilos,Mandelbrot,Convolucion" > resultados/tiempos_auto.csv

for t in 1 2 3 4 5 6 7 8
do
    export OMP_NUM_THREADS=$t
    echo "Ejecutando con $t hilos..."

    ./openmp_base | awk -v h=$t '
    /Tiempo Mandelbrot:/ {m=$3}
    /Tiempo Convolucion:/ {c=$3}
    END {print h "," m "," c}
    ' >> resultados/tiempos_auto.csv
done

echo "Resultados guardados en resultados/tiempos_auto.csv"
