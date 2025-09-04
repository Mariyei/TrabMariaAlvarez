En esta carpeta se encuentran los \*\*datos\*\* y \*\*scripts\*\* utilizados para analizar la latencia en los distintos esquemas de seguridad implementados en los dispositivos ESP32-WROOM-32.



\## 📂 Datos Recopilados

Se dividen en tres carpetas, una para cada esquema de seguridad. Dentro de cada una encontrarás:

\- `Emisor\_\*.csv`: marcas de tiempo al enviar mensajes.

\- `Receptor\_\*.csv`: marcas de tiempo al recibir mensajes.

\- `Latencia\_\*.csv`: latencia obtenida a partir de los datos de emisor y receptor utilizando el script de Python `latencia\_\*.py`.



\## 📂 Scripts Utilizados

Contiene los scripts de Python empleados para:

\- `latencia\_\*.py`: cálculo de la latencia a partir de los registros de emisor y receptor.

\- `estadisticos\_\*.py`: cálculo de indicadores estadísticos (media, moda, percentiles, etc.).



\## Nota

Los resultados finales no están incluidos aquí; únicamente los datos recopilados y los scripts necesarios para reproducir el análisis.

