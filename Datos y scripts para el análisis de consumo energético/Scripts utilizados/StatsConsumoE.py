import pandas as pd
import numpy as np
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

Resultados = []

for nombre, ruta in Archivos.items():
    # Leer CSV
    Datos = pd.read_csv(ruta)

    # Filtrar solo hasta ID = 6000
    DatosExt = Datos[Datos['ID'] <= 6000]
    print(f"{nombre}: {len(DatosExt)} registros procesados (ID <= 6000)")

    # Extraer Corriente_mA
    Corriente = DatosExt['Corriente_mA'].dropna()

    # Calcular estadísticos
    Media = Corriente.mean()
    Mediana = Corriente.median()
    Modas = Corriente.mode()  # devuelve una serie con todas las modas
    modas_str = ", ".join([str(round(m, 3)) for m in Modas])  # las convertimos en texto
    DesvEs = Corriente.std()
    Min = Corriente.min()
    Max = Corriente.max()
    Corriente_estable = DatosExt.loc[DatosExt['ID'] > 300, 'Corriente_mA'].dropna()
    min_estable = Corriente_estable.min()
    p25 = np.percentile(Corriente, 25)
    p75 = np.percentile(Corriente, 75)

    # Guardar Resultados
    Resultados.append({
        "Esquema": nombre,
        "Media (mA)": round(Media, 3),
        "Mediana (mA)": round(Mediana, 3),
        "Moda (mA)": modas_str,
        "Desv. Est.": round(DesvEs, 3),
        "Mínimo (mA)": round(Min, 3),
        "Máximo (mA)": round(Max, 3),
        "Min. estable (mA)": round(min_estable, 3),
        "P25 (mA)": round(p25, 3),
        "P75 (mA)": round(p75, 3)
    })

# Se crea un dataFrame con todos los Resultados
DatosResultados = pd.DataFrame(Resultados)

# Mostrar tabla
pd.set_option('display.max_columns', None)  
pd.set_option('display.width', None)        
print(DatosResultados)


# Se guardan los resultados en un CSV.
#DatosResultados.to_csv("Resultados Estadisticos mA en Emisor.csv", index=False)
DatosResultados.to_csv("Resultados Estadisticos mA en Receptor.csv", index=False)
