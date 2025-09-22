import pandas as pd
from scipy import stats

# Rutas de archivo para emisores
#Archivos = {
#    "Sin cifrado": "RegistrodeConsumoEnergéticoEmisorSinCifrado.csv",
#    "TLS": "RegistrodeConsumoEnergéticoEmisorconTLS.csv",
#    "AES+ECC": "RegistrodeConsumoEnergéticoEmisorconAESyECC.csv"
#}

# Rutas de archivo para receptores (activar si se desea probar con receptores)
Archivos = {
     "Sin cifrado": "RegistrodeConsumoEnergéticoReceptorSinCifrado.csv",
     "TLS": "RegistrodeConsumoEnergéticoReceptorconTLS.csv",
     "AES+ECC": "RegistrodeConsumoEnergéticoReceptorconAESyECC.csv"
}

# Se crea un diccionario para almacenar las corrientes por esquema
corrientes = {}

for nombre, ruta in Archivos.items():
    # Leer el archivo CSV para cada esquema
    Datos = pd.read_csv(ruta)

    # Se filtrar solo los registros con ID <= 6000
    DatosExt = Datos[Datos['ID'] <= 6000]
    print(f"{nombre}: {len(DatosExt)} registros procesados (ID <= 6000)")

    # Se extraer la columna 'Corriente_mA' y eliminar los valores nulos
    Corriente = DatosExt['Corriente_mA'].dropna()

    # Se almacenar los datos de corriente para cada esquema
    corrientes[nombre] = Corriente

# Prueba de Levene para homogeneidad de varianzas
levene_stat, levene_p = stats.levene(corrientes['Sin cifrado'],
                                     corrientes['TLS'],
                                     corrientes['AES+ECC'])

print("\nPrueba de Levene para homogeneidad de varianzas")
print(f"Estadístico de Levene: {levene_stat:.3f}")
print(f"Valor p: {levene_p:.5f}")

if levene_p > 0.05:
    print("No se rechaza H0: se asume homogeneidad de varianzas.")
else:
    print("Se rechaza H0: no hay homogeneidad de varianzas.")
