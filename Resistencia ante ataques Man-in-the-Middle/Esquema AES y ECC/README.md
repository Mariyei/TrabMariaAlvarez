# Procedimiento realizado para las pruebas de resistencia ante ataque Man-in-the-Middle (MitM) en esquema combinado de AES y ECC a nivel de aplicación. 

## Configuraciones iniciales

En esta ejecución de pruebas, se utilizan las mismas configuraciones iniciales aplicadas en el esquema sin cifrado y de habilitación de TLS.

⚠️ Aviso importante
Los comandos y configuraciones presentados en este repositorio tienen únicamente fines académicos y de investigación. No deben emplearse para actividades ilícitas ni en entornos fuera de pruebas controladas.

## Wireshark: comandos utilizados

Los comandos utilizados en esta sección, corresponden a los mismos comandos utilizados en el esquema sin cifrado y de habilitación de TLS, aunque se presentan ciertas diferencias, que a continuación serán explicadas: 

En este caso conocemos que la interfaz tiene el nombre de `wlan0`, el cual se encuentra en modo managed, y se debe trabaja en modo monitor, por lo que se utiliza el comando `sudo airmon-ng start wlan0` y automáticamente la interfaz adquiere el nombre de `wlan0mon`.

![Comando 1-3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/Comando%201-3.png)

Esto al igual que en los dos casos anteriores, se puede comprobar con el comando `ip a`, el cual muestra las interfaces de red disponibles. 

![Comando 2-3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/Comando%202-3.png)

Una vez que se ha realizado esto, se utiliza el comando `sudo airodump-ng wlan0mon`, que permite monitorear el tráfico inalámbrico en el canal donde opera la red de interés. Al igual que el esquema de cifrado y de habilitación de TLS, se listan los clientes asociados al Access Point, indicando su dirección MAC, el BSSID al que se conectan y el tráfico generado. 

![Comando 3-3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/Comando%203-3.png)

Para este caso, se obtienen las direcciones MAC desde el modo monitor y se verifica en que canal están. 

- Dirección MAC del Emisor: 

![Reporte 1-3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/Reporte%201-3.png)

- Dirección MAC del Receptor: 

![Reporte 2-3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/Reporte%202-3.png)

Una diferencia es que para este esquema, los dispositivos se encuentran en el canal 1, a diferencia del esquema sin cifrado, que los dispositivos estaban en el canal 6 y en el esquema de TLS en el canal 11. 

Luego, ejecutamos el comando `sudo airodump-ng wlan0mon -c 1`, para únicamente recibir el tráfico en el canal 1. 

![Comando 4-3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/Comando%204-3.png)

Una vez que se realiza esto, se inicia la aplicación de Wireshark para la escucha del tráfico en la interfaz `wlan0mon`.  

A continuación se utiliza el comando `sudo aireplay-ng --deauth 1 -a <MAC DEL ROUTER> -c <MAC DEL ESP32> wlan0mon`, para forzar la desconexión en ambos dispositivos, el emisor y receptor. 

![Comando 5-6-3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/Comando%205-6-3.png)

A continuación una imagen que representa el momento de desconexión de parte del emisor, y ocurre lo mismo con el receptor: 

![DesconexionEmisor](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/DesconexionEmisor.png)

Una vez que se reconectan los dispositivos, se tiene el handshake WPA2, el cual se encuentra con el filtro en Wireshark de `EAPOL`. 

![EAPOL](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/EAPOL.png)

Al mismo tiempo, podemos observar que al momento de reconectarse, los dos dispositivos realizan el intercambio de claves ECC, para generar la nueva clave AES. 

- Clave generada a partir de la reconexión de emisor: 

![ReconexionEmisor1](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/ReconexionEmisor1.png)

![ReconexionReceptor1](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/ReconexionReceptor1.png)

- Clave generada a partir de la reconexión del receptor: 

![ReconexionReceptor2](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/ReconexionEmisor2.png)

![ReconexionEmisor2](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20AES%20y%20ECC/Material%20visual/Reconexi%C3%B3nReceptor2.png)

Finalmente, el análisis de los paquetes recopilados después de este handshake y reconexión de los dispositivos, se presenta directamente en el documento del trabajo de graduación.

