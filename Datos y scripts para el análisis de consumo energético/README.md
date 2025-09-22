En esta carpeta se encuentran los **datos**, **scripts** y **resultados obtenidos** para analizar el consumo energético en los distintos esquemas de seguridad implementados en los ESP32-WROOM-32.

## 📂 Datos Recopilados

Se dividen en tres carpetas, una para cada esquema de seguridad. Dentro de cada carpeta, existen dos archivos: 

- `RegistrodeConsumoEnergéticoEmisor_*.csv`: contiene la información de voltaje, amperaje (mA), Potencia (mW), Miliamperios por hora (mAh) y Potencia por hora (mWh) relacionada al emisor.

- `RegistrodeConsumoEnergéticoReceptor_*.csv`: contiene la información de voltaje, amperaje (mA), Potencia (mW), Miliamperios por hora (mAh) y Potencia por hora (mWh) relacionada al receptor.

## 📂 Scripts Utilizados

Contiene los scripts de Python utilizados: 
- `StatsConsumoE.py`: realiza el cálculo de indicadores estadísticos (media, mediana, moda, etc.). 

- `StatsmAh.py`: realiza principalmente el gráfico acumulado de los miliamperios por hora consumidos por 10 minutos. 

- `PruebaLeneveCE.py`: ejecuta la prueba de Levene para verificar la homogeneidad de varianzas en los datos de consumo energético.

- `AnalisisAnova.py`: aplica el análisis de varianza (ANOVA) para comparar el consumo entre los esquemas de seguridad.

- `AnalisisTStudent.py`: realiza pruebas t de Student para contrastar diferencias de consumo entre pares de esquemas.

## 📂 Resultados obtenidos

Contiene los resultados obtenidos a partir de los scripts de Python: 
- `Resultados Estadísticos mA en Emisor.csv`: contiene los resultados estadísticos del emisor por cada esquema planteado. 

- `Resultados Estadísticos mA en Receptor.csv`: contiene los resultados estadísticos del receptor por cada esquema planteado. 

- `ResultadosmAhEmisor.csv`: contiene los resultados relacionados con los datos de miliamperios-Hora del emisor. 

- `ResultadosmAhReceptor.csv`: contiene los resultados relacionados con los datos de miliamperios-Hora del receptor. 


## Nota
El análisis final no está incluido aquí; únicamente se presentan los datos recopilados, los scripts utilizados y los resultados obtenidos a partir de dichos scripts, pero no se explican. 