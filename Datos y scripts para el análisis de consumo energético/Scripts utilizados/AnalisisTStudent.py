import pandas as pd
from scipy import stats

#Rutas de archivo para emisores
#Archivos = {
#    "Sin cifrado": "RegistrodeConsumoEnergéticoEmisorSinCifrado.csv",
#    "TLS": "RegistrodeConsumoEnergéticoEmisorconTLS.csv",
#    "AES+ECC": "RegistrodeConsumoEnergéticoEmisorconAESyECC.csv"
#}

#Rutas de archivo para receptores
Archivos = {
    "Sin cifrado": "RegistrodeConsumoEnergéticoReceptorSinCifrado.csv",
    "TLS": "RegistrodeConsumoEnergéticoReceptorconTLS.csv",
    "AES+ECC": "RegistrodeConsumoEnergéticoReceptorconAESyECC.csv"
}

# Crear un diccionario para almacenar las corrientes por esquema
corrientes = {}

# Leer y filtrar los datos
for nombre, ruta in Archivos.items():
    # Leer el archivo CSV para cada esquema
    Datos = pd.read_csv(ruta)

    # Filtrar solo los registros con ID <= 6000
    DatosExt = Datos[Datos['ID'] <= 6000]
    print(f"{nombre}: {len(DatosExt)} registros procesados (ID <= 6000)")

    # Extraer la columna 'Corriente_mA' y eliminar los valores nulos
    Corriente = DatosExt['Corriente_mA'].dropna()

    # Almacenar los datos de corriente para cada esquema
    corrientes[nombre] = Corriente

# TLS vs AES+ECC
t_TLSvsAESyECC, p_TLSvsAESyECC = stats.ttest_ind(corrientes['TLS'], corrientes['AES+ECC'], equal_var=False)

# Sin cifrado vs TLS
t_SinCifradovsTLS, p_SinCifradovsTLS = stats.ttest_ind(corrientes['Sin cifrado'], corrientes['TLS'], equal_var=False)

# Sin cifrado vs AES+ECC
t_SinCifradovsAESyECC, p_SinCifradovsAESyECC = stats.ttest_ind(corrientes['Sin cifrado'], corrientes['AES+ECC'], equal_var=False)

# Resultados
print("\nResultados de la prueba T-Student")
print(f"1. TLS vs ECC+AES: t = {t_TLSvsAESyECC:.3f}, p = {p_TLSvsAESyECC:.5f}")
print(f"2. Sin cifrado vs TLS: t = {t_SinCifradovsTLS:.3f}, p = {p_SinCifradovsTLS:.5f}")
print(f"3. Sin cifrado vs ECC+AES: t = {t_SinCifradovsAESyECC:.3f}, p = {p_SinCifradovsAESyECC:.5f}")