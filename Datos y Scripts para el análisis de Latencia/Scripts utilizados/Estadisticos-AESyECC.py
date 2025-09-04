import pandas as pd
import matplotlib.pyplot as plt

#Se localiza el archivo con los datos para procesarlos
Datos = pd.read_csv('Latencia-AESyECC.csv')  #Ruta del archivo a procesar

# Si es necesario, puedes convertir los valores de 'latencia' a números
Datos['latencia'] = pd.to_numeric(Datos['latencia'], errors='coerce')

# Estadísticas descriptivas (redondeadas a 2 decimales)
Media = round(Datos['latencia'].mean(), 2)
DesvEstandar = round(Datos['latencia'].std(), 2)
LatenciaMinima = round(Datos['latencia'].min(), 2)
LatenciaMaxima = round(Datos['latencia'].max(), 2)
conteo = Datos['latencia'].value_counts()
max_frecuencia = conteo.max()
Modas = [round(x, 2) for x in conteo[conteo == max_frecuencia].index.tolist()]

# Percentiles
Percentil25 = round(Datos['latencia'].quantile(0.25), 2)
Percentil50 = round(Datos['latencia'].quantile(0.50), 2)
Percentil75 = round(Datos['latencia'].quantile(0.75), 2)

# Se imprimen los resultados
print("Estadísticas descriptivas de latencia:")
print(f"Media: {Media} ms")
if len(Modas) == 1:
    print(f"Moda: {Modas[0]} ms (se repite {max_frecuencia} veces)")
else:
    print(f"Modas: {Modas} ms (cada una se repite {max_frecuencia} veces)")
print(f"Desviación estándar: {DesvEstandar} ms")
print(f"Latencia mínima: {LatenciaMinima} ms")
print(f"Latencia máxima: {LatenciaMaxima} ms")
print(f"Percentil 25: {Percentil25} ms")
print(f"Percentil 50 (Mediana): {Percentil50} ms")
print(f"Percentil 75: {Percentil75} ms")
