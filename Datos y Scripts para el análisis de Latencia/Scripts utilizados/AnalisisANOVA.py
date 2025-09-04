import pandas as pd
from scipy import stats

# Datos de cada esquema
SinCifrado = pd.read_csv('LatenciaSinCifrado.csv')['latencia']
TLS = pd.read_csv('LatenciaConTLS.csv')['latencia']
ECCyAES = pd.read_csv('LatenciaAESyECC.csv')['latencia']

# ANOVA de un factor
f_statistic, p_value = stats.f_oneway(SinCifrado, TLS, ECCyAES)

print("ANOVA de un factor")
print(f"Estadístico F: {f_statistic:.3f}")
print(f"Valor p: {p_value:.5f}")