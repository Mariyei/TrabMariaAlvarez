En esta carpeta se encuentran los **datos** y **scripts** utilizados para analizar la latencia en los distintos esquemas de seguridad implementados en los dispositivos ESP32-WROOM-32.

## 📂 Datos Recopilados

Se dividen en tres carpetas, una para cada esquema de seguridad. Dentro de cada una encontrarás:

- `Emisor_*.csv`: marcas de tiempo al enviar mensajes.

- `Receptor_*.csv`: marcas de tiempo al recibir mensajes.

- `Latencia_*.csv`: latencia obtenida a partir de los datos de emisor y receptor utilizando el script de Python `latencia_*.py`.



## 📂 Scripts Utilizados

Contiene los scripts de Python empleados para:

- `Latencia_*.py`: cálculo de la latencia a partir de los registros de emisor y receptor.

- `Estadisticos_*.py`: cálculo de indicadores estadísticos (media, moda, percentiles, etc.).

- `AnalisisANOVA.py`: análisis de varianza (ANOVA) para comparar las diferencias de latencia entre los distintos esquemas de seguridad.

- `AnalisisTStudent.py`: prueba t de Student para evaluar la significancia estadística entre pares de esquemas de seguridad.


## Nota

Los resultados finales no están incluidos aquí; únicamente los datos recopilados y los scripts necesarios para reproducir el análisis.

