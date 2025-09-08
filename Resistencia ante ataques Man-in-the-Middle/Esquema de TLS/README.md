# Procedimiento realizado para las pruebas de resistencia ante ataque Man-in-the-Middle (MitM) en esquema de habilitación de TLS. 

## Configuraciones iniciales

En esta ejecución de pruebas, se utilizan las mismas configuraciones iniciales aplicadas en el esquema sin cifrado. 

⚠️ Aviso importante
Los comandos y configuraciones presentados en este repositorio tienen únicamente fines académicos y de investigación. No deben emplearse para actividades ilícitas ni en entornos fuera de pruebas controladas.

## Wireshark: comandos utilizados

Los comandos utilizados en esta sección, corresponden a los mismos comandos utilizados en el esquema sin cifrado, aunque se presentan ciertas diferencias, que a continuación serán explicadas: 

Inicialmente se utiliza el comando `iwconfig`, el cual es un comando que nos permite verificar las interfaces inalámbricas disponibles y muestra parámetros como: SSID, frecuencia de operación, canal, potencia de la señal, etc. En este caso, la interfaz sigue siendo `wlan0`. 

![Comando 2-1](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Reporte1-1.png)

Al igual que el esquema sin cifrado, se trabaja con la interfaz de modo managed a modo monitor, se realiza con el comando `sudo airmon-ng start wlan0`, y tiene adquiere el mismo nombre de `wlan0mon`. 

![Comando 2-2](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Comandos2-2.png)

Una vez que se ha realizado esto, se utiliza el comando `sudo airodump-ng wlan0mon`, que permite monitorear el tráfico inalámbrico en el canal donde opera la red de interés. Al igual que el esquema de cifrado, se listan los clientes asociados al Access Point, indicando su dirección MAC, el BSSID al que se conectan y el tráfico generado. 

Para este caso, se obtienen las direcciones MAC desde el modo managed. 

- Dirección MAC del Emisor: 

![Reporte 1-1](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Reporte1-1.png)

- Dirección MAC del Receptor: 

![Reporte 1-2](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Reporte1-2.png)

Una diferencia es que para este esquema, los dispositivos se encuentran en el canal 11, a diferencia del esquema sin cifrado, que los dispositivos están en el canal 6. 

Primero ejecutamos el comando `sudo airodump-ng --bssid <MAC DEL ROUTER> --channel 6 wlan0mon`, para unicamente recibir el tráfico en el canal 6, y no se encuentra ningún comando. 

![Comando 3-2](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Comandos3-2.png)

Luego se ejecuta el comando `sudo airodump-ng --bssid <MAC DEL ROUTER> --channel 11 wlan0mon`. En el cual se encuentran los dispositivos emisor y receptor. 

![Comando 4-2](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Comandos4-2.png)

A continuación se utiliza el comando `sudo aireplay-ng --deauth 5 -a <MAC DEL ROUTER> -c <MAC DEL ESP32> wlan0mon`, para forzar la desconexión a ambos dispositivos, al emisor y receptor. 

![Comando 5y6-2](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Comandos5y6-2.png)

Una vez que se reconectan los dispositivos, se tiene el handshake TLS, el cual se encuentra como un mensaje `Server Hello`, en cual se acuerda como se comunican de forma segura, confirma la versión y que algoritmos serán utilizados. 

- Mensaje `Server Hello` del emisor: 

![Reporte 1-3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Reporte%201-3.png)

- Mensaje `Server Hello` del receptor: 

![Reporte 1-4](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20de%20TLS/Material%20visual/Reporte%201-4.png)

Con esto se completa la captura del proceso de reconexión bajo el esquema con TLS, en el cual se identifican los mensajes Server Hello tanto del emisor como del receptor. Dichos mensajes forman parte del procedimiento de handshake y confirman la negociación de los parámetros de la sesión segura.

Finalmente, el análisis de los paquetes recopilados después de este handshake, se presenta directamente en el documento del trabajo de graduación.