#!/bin/bash

echo "Afinidad,Mandelbrot,Convolucion,HistogramaLocal" > resultados/afinidad_auto.csv

export OMP_NUM_THREADS=4

unset OMP_PROC_BIND
unset OMP_PLACES

./vectorizacion | awk '
/Tiempo Mandelbrot:/ {m=$3}
/Tiempo Convolucion:/ {c=$3}
/Histograma local:/ {h=$3}
END {print "sin_afinidad," m "," c "," h}
' >> resultados/afinidad_auto.csv

export OMP_PROC_BIND=close
export OMP_PLACES=cores

./vectorizacion | awk '
/Tiempo Mandelbrot:/ {m=$3}
/Tiempo Convolucion:/ {c=$3}
/Histograma local:/ {h=$3}
END {print "close," m "," c "," h}
' >> resultados/afinidad_auto.csv

export OMP_PROC_BIND=spread
export OMP_PLACES=cores

./vectorizacion | awk '
/Tiempo Mandelbrot:/ {m=$3}
/Tiempo Convolucion:/ {c=$3}
/Histograma local:/ {h=$3}
END {print "spread," m "," c "," h}
' >> resultados/afinidad_auto.csv

echo "Resultados guardados en resultados/afinidad_auto.csv"
