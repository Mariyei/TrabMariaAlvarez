import pandas as pd
from scipy import stats

#Rutas de archivo para emisores
Archivos = {
    "Sin cifrado": "RegistrodeConsumoEnergéticoEmisorSinCifrado.csv",
    "TLS": "RegistrodeConsumoEnergéticoEmisorconTLS.csv",
    "AES+ECC": "RegistrodeConsumoEnergéticoEmisorconAESyECC.csv"
}

#Rutas de archivo para receptores
#Archivos = {
#    "Sin cifrado": "RegistrodeConsumoEnergéticoReceptorSinCifrado.csv",
#    "TLS": "RegistrodeConsumoEnergéticoReceptorconTLS.csv",
#    "AES+ECC": "RegistrodeConsumoEnergéticoReceptorconAESyECC.csv"
#}

# Crear un diccionario para almacenar las corrientes por esquema
corrientes = {}

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

# ANOVA de un factor para comparar las corrientes entre los tres esquemas
f_statistic, p_value = stats.f_oneway(corrientes['Sin cifrado'], corrientes['TLS'], corrientes['AES+ECC'])

# Resultados del ANOVA
print("\nANOVA de un factor para las corrientes")
print(f"Estadístico F: {f_statistic:.3f}")
print(f"Valor p: {p_value:.5f}")