import pandas as pd
from scipy import stats

# Datos de latencia de cada esquema
SinCifrado = pd.read_csv('Latencia-SinCifrado.csv')['latencia']
TLS = pd.read_csv('Latencia-TLS.csv')['latencia']
ECCyAES = pd.read_csv('Latencia-AESyECC.csv')['latencia']

# TLS vs ECC+AES
t_TLSvsAESyECC, p_TLSvsAESyECC = stats.ttest_ind(TLS, ECCyAES, equal_var=False)

# Sin cifrado vs TLS
t_SinCifradovsTLS, p_SinCifradovsTLS = stats.ttest_ind(SinCifrado, TLS, equal_var=False)

# Sin cifrado vs ECC+AES
t_SinCifradovsAESyECC, p_SinCifradovsAESyECC = stats.ttest_ind(SinCifrado, ECCyAES, equal_var=False)

# Resultados
print("Resultados de la prueba T-Student")
print(f"1. TLS vs ECCyAES: t = {t_TLSvsAESyECC:.3f}, p = {p_TLSvsAESyECC:.5f}")
print(f"2. Sin cifrado vs TLS: t = {t_SinCifradovsTLS:.3f}, p = {p_SinCifradovsTLS:.5f}")
print(f"3. Sin cifrado vs ECCyAES: t = {t_SinCifradovsAESyECC:.3f}, p = {p_SinCifradovsAESyECC:.5f}")