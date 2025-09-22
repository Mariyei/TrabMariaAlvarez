import pandas as pd
from scipy import stats

# Cargar datos de latencia para cada esquema
SinCifrado = pd.read_csv('LatenciaSinCifrado.csv')['latencia'].dropna()
TLS = pd.read_csv('LatenciaTLS.csv')['latencia'].dropna()
ECCyAES = pd.read_csv('LatenciaAESyECC.csv')['latencia'].dropna()

# Mostrar tamaños de muestra
print("Tamaños de muestra:")
print(f"Sin cifrado: {len(SinCifrado)} registros")
print(f"TLS: {len(TLS)} registros")
print(f"AES+ECC: {len(ECCyAES)} registros\n")

# (Opcional) Prueba de Levene para homogeneidad de varianzas
levene_stat, levene_p = stats.levene(SinCifrado, TLS, ECCyAES)
print("\nPrueba de Levene para homogeneidad de varianzas")
print(f"Estadístico de Levene: {levene_stat:.3f}")
print(f"Valor p: {levene_p:.5f}")
if levene_p > 0.05:
    print("No se rechaza H0: se asume homogeneidad de varianzas.")
else:
    print("Se rechaza H0: no hay homogeneidad de varianzas.")
