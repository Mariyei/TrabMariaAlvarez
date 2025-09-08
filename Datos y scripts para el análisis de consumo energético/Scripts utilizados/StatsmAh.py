import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

plt.rcParams["font.family"] = "Times New Roman"

#Rutas de archivo para emisores
#Archivos = {
#    "Sin cifrado": "RegistrodeConsumoEnergéticoEmisorSinCifrado.csv",
#   "TLS": "RegistrodeConsumoEnergéticoEmisorconTLS.csv",
#   "AES+ECC": "RegistrodeConsumoEnergéticoEmisorconAESyECC.csv"
#}

#Rutas de archivo para receptores
Archivos = {
    "Sin cifrado": "RegistrodeConsumoEnergéticoReceptorSinCifrado.csv",
   "TLS": "RegistrodeConsumoEnergéticoReceptorconTLS.csv",
   "AES+ECC": "RegistrodeConsumoEnergéticoReceptorconAESyECC.csv"
}

Resultados = []
# Segundos (1 a 10 min)
Hitos = [60, 120, 180, 240, 300, 360, 420, 480, 540, 600]

for nombre, ruta in Archivos.items():
    # Se lee el CSV
    Datos = pd.read_csv(ruta)

    # Se filtran los datos hasta ID = 6000 (10 minutos a 100 ms por muestra)
    DatosExt = Datos[Datos['ID'] <= 6000].reset_index(drop=True)

    # Se calcula el tiempo en segundos (ID * 0.1s, porque cada muestra es 100 ms)
    DatosExt['Tiempo_s'] = DatosExt['ID'] * 0.1

    # mAh final 
    mAhFinal = DatosExt['mAh'].iloc[-1]

    # mAh por minuto
    mAhMin = mAhFinal / 10

    # mAh en cada minuto
    valoresHitos = {}
    for h in Hitos:
        valor = np.interp(h, DatosExt['Tiempo_s'], DatosExt['mAh'])
        valoresHitos[f"{h//60} min"] = round(valor, 3)

    # Se guardan los resultados
    fila = {"Esquema": nombre, "mAh final": round(mAhFinal, 3), "mAh/min": round(mAhMin, 3)}
    fila.update(valoresHitos)
    Resultados.append(fila)

# Se crea la tabla con resultados
DatosResultados = pd.DataFrame(Resultados)

# Se muestra la tabla
print(DatosResultados)

# Se guardan los resultados en un CSV
#DatosResultados.to_csv("ResultadosmAhReceptor.csv", index=False)
#DatosResultados.to_csv("ResultadosmAhEmisor.csv", index=False)

plt.figure(figsize=(10, 5))

# Gráfico comparativo con todos los minutos
for nombre in Archivos.keys():
    y_vals = DatosResultados[DatosResultados['Esquema'] == nombre].iloc[0, 3:].values
    plt.plot(range(1, 11), y_vals, marker="o", label=nombre)

plt.ylabel("mAh")
plt.xlabel("Tiempo (minutos)")
plt.xticks(range(1, 11))
plt.legend()
plt.grid(True)
ax = plt.gca()
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)

#plt.savefig("mAh acumulado por minuto - Emisor.png", dpi=300, bbox_inches="tight")
plt.savefig("mAh acumulado por minuto - Receptor.png", dpi=300, bbox_inches="tight")

plt.show()
