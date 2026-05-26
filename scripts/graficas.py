import csv
import os
import matplotlib.pyplot as plt

os.makedirs("resultados/graficas", exist_ok=True)

# -------------------------
# Tiempo vs hilos
# -------------------------
hilos = []
mandelbrot = []
convolucion = []

with open("resultados/tiempos.csv") as f:
    reader = csv.DictReader(f)
    for row in reader:
        hilos.append(int(row["Hilos"]))
        mandelbrot.append(float(row["Mandelbrot"]))
        convolucion.append(float(row["Convolucion"]))

plt.figure()
plt.plot(hilos, mandelbrot, marker="o", label="Mandelbrot")
plt.plot(hilos, convolucion, marker="o", label="Convolución")
plt.xlabel("Número de hilos")
plt.ylabel("Tiempo de ejecución (s)")
plt.title("Tiempo de ejecución vs número de hilos")
plt.legend()
plt.grid(True)
plt.savefig("resultados/graficas/tiempo_vs_hilos.png")

# -------------------------
# Speedup
# -------------------------
base_m = mandelbrot[0]
base_c = convolucion[0]

speedup_m = [base_m / t for t in mandelbrot]
speedup_c = [base_c / t for t in convolucion]

plt.figure()
plt.plot(hilos, speedup_m, marker="o", label="Mandelbrot")
plt.plot(hilos, speedup_c, marker="o", label="Convolución")
plt.xlabel("Número de hilos")
plt.ylabel("Speedup")
plt.title("Speedup vs número de hilos")
plt.legend()
plt.grid(True)
plt.savefig("resultados/graficas/speedup_vs_hilos.png")

# -------------------------
# Schedulers
# -------------------------
labels = []
times = []

with open("resultados/scheduler_tests.csv") as f:
    reader = csv.DictReader(f)
    for row in reader:
        labels.append(row["Scheduler"] + "-" + row["Chunk"])
        times.append(float(row["Mandelbrot"]))

plt.figure(figsize=(12, 5))
plt.bar(labels, times)
plt.xlabel("Scheduler")
plt.ylabel("Tiempo Mandelbrot (s)")
plt.title("Comparación de schedulers OpenMP")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.savefig("resultados/graficas/schedulers.png")

# -------------------------
# Histograma
# -------------------------
metodos = []
hist_tiempos = []

with open("resultados/histograma.csv") as f:
    reader = csv.DictReader(f)
    for row in reader:
        metodos.append(row["Metodo"])
        hist_tiempos.append(float(row["Tiempo"]))

plt.figure()
plt.bar(metodos, hist_tiempos)
plt.xlabel("Método")
plt.ylabel("Tiempo (s)")
plt.title("Comparación de histograma")
plt.grid(True, axis="y")
plt.savefig("resultados/graficas/histograma.png")

# -------------------------
# Afinidad
# -------------------------
afinidad = []
afinidad_m = []
afinidad_c = []

with open("resultados/afinidad.csv") as f:
    reader = csv.DictReader(f)
    for row in reader:
        afinidad.append(row["Afinidad"])
        afinidad_m.append(float(row["Mandelbrot"]))
        afinidad_c.append(float(row["Convolucion"]))

plt.figure()
plt.plot(afinidad, afinidad_m, marker="o", label="Mandelbrot")
plt.plot(afinidad, afinidad_c, marker="o", label="Convolución")
plt.xlabel("Configuración de afinidad")
plt.ylabel("Tiempo (s)")
plt.title("Impacto de afinidad de hilos")
plt.legend()
plt.grid(True)
plt.savefig("resultados/graficas/afinidad.png")

print("Gráficas generadas en resultados/graficas/")
