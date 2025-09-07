# Procedimiento realizado para las pruebas de resistencia ante ataque Man-in-the-Middle (MitM)

## Configuraciones iniciales

### Sistema operativo: Kali Linux

Se utilizó principalmente este entorno debido a que incluye una amplia gama de herramientas y utilidades que permite a los profesionales de seguridad y  TI evaluar y medir la seguridad de sus sistemas.

Entre sus herramientas, tenemos Wireshark y Scapy por defecto, por lo que se requieren instalaciones adicionales. 

### Conexión dentro de la red de pruebas

Los dispositivos se encuentran conectados dentro de una red doméstica, la cual se empleó durante todo el proyecto. Para efectos de este trabajo, la máquina utilizada para realizar las pruebas de Man-in-the-Middle se encontraba dentro de la misma red doméstica, desde donde se llevaron a cabo los ataques de prueba.

### Proceso y resultados iniciales del ataque MitM (Esquema Sin Cifrado). 

En primera instancia, se investigó cómo es posible acceder al tráfico de una red sin necesidad de conocer credenciales de usuario ni contraseñas o alguna información adicional.

Para este trabajo, únicamente se ejecutó un ataque MitM de tipo pasivo, con el objetivo de demostrar:

- La localización y lectura de información en los paquetes transmitidos.
- La interceptación de mensajes y datos que viajan sobre la red.
- Cómo, en caso de que un tercero acceda a esta información, se puede comprometer la seguridad del sistema.

Este ejercicio evidencia la facilidad con la que los dispositivos IoT pueden ser vulnerados, lo que podría dar lugar a ataques más severos, como ya ha sucedido en diversos incidentes documentados.

Se utilizó información obtenida en foros, servidores de internet y herramientas de inteligencia artificial para orientar la práctica.


⚠️ Aviso importante
Los comandos y configuraciones presentados en este repositorio tienen únicamente fines académicos y de investigación. No deben emplearse para actividades ilícitas ni en entornos fuera de pruebas controladas.

## Wireshark: Comandos utilizados

### Comando 1: 

El comando `ip a` proporciona información sobre todas las interfaces disponibles en el sistema junto con sus direcciones IP y estado de conexión. Este paso permite ubicar la interfaz de red de la computadora, en este caso **wlan0**.
 

![Comando 1](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%201.png)

### Comando 2: 

El comando `iw dev wlan0 link` muestra el estado de la interfaz `wlan0`, indicando a qué red Wi-Fi está conectada, la intensidad de la señal y la frecuencia de operación.  Estos datos son relevantes para el análisis de dispositivos IoT y su comunicación dentro de la red.


![Comando 2](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%202.png)

### Comando 3: 

El comando `sudo airmon-ng start wlan0` cambia la interfaz a **modo monitor**, lo que permite capturar tráfico inalámbrico en el aire. En este modo se observan paquetes intercambiados, intentos de conexión y actividad general de la red Wi-Fi. Al activarse, la interfaz se renombra como **wlan0mon**, y la computadora pierde conexión a internet temporalmente.

![Comando 3](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%203.png)

Se verifica nuevamente con `ip a` que la interfaz cambió de nombre de `wlan0` a `wlan0mon`. Cabe mencionar que en este estado no es posible navegar por internet.

![Comando 1-2-4](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%202.png)

Una vez realizado este proceso, se abre la aplicación Wireshark para comenzar la captura de tráfico en la interfaz en modo monitor. Puede abrirse desde consola con el comando `sudo wireshark`.

![Comando Wireshark](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%20Wireshark.png)

### Comando 4: 

Se reutiliza el **Comando 2** `iw dev wlan0mon link`, el cual debe devolver un estado de "Not connected", confirmando que se encuentra en estado monitor. 

Posteriormente, se utiliza el comando `sudo airodump-ng wlan0mon`, el cual inicia la captura de paquetes en la interfaz wlan0mon. Muestra en tiempo real las redes Wi-Fi cercanas (SSID,  tipo de cifrado), direcciones MAC. Con esto se puede identificar a qué canal y punto de acceso se conectan los dispositivos. 

En este caso, se localiza el router de la red doméstica, y algunos puntos hacia donde se dirige el tráfico. Inicialmente, presuntamente los dispositivos ESP32-WROOM-32. La dirección MAC del router corresponde al encuadre rojo señalado y oculto por temas de seguridad. 

![Comando 2-2-4](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%202-2-4.png)

### Comando 5: 

Se realiza brevemente una revisión de paquetes capturados en Wireshark hasta el momento, y se extraen de forma exitosa las direcciones MAC de los nodos ESP32-WROOM-32, tanto del emisor como del receptor. 

La imagen a continuación muestra que la dirección MAC de la parte emisora corresponde a AC:15:18:D6:85:B0. Se identifica fácilmente, debido a que la columna de "source" y "destination" de Wireshark, las muestran como Espressif_X:X:X, y se sabe que es la empresa productora de los ESP32, lo que apunta directamente a los dispositivos. 

- A continuación la captura de dirección MAC del emisor: 

Insertar Imagen CapturadeMACEMISOR. 
![CapturadeMACEMISOR](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/CapturadeMACEMISOR.png)

- En cambio, la dirección MAC del receptor, también localizada de la misma forma, corresponde a 34:5F:A9:B9:74: 

![CapturadeMACReceptor](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/CapturadeMACReceptor.png) 

Una vez que se tiene esta información, en la cual se extrae información sobre el canal en el que viajan estos paquetes de datos, que corresponde al  canal 6. Se utiliza el comando `sudo airodump-ng wlan0mon -c 6`. 

Dicho comando nos proporciona información del tráfico que ocurre únicamente en el canal 6, reduciendo el ruido y mostrando únicamente las redes y dispositivos que operan en dicho canal. Facilita el análisis de paquetes. Se localizan los dos nodos, tanto el emisor como el receptor. 

![Comando 5](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%205.png)

### Comando 6: 

El comando `sudo aireplay-ng --deauth 10 -a <MAC ROUTER> -c <MAC DISPOSITIVO> wlan0mon` envía 10 paquetes de desautenticación al dispositivo objetivo.  
Este procedimiento se aplicó tanto al emisor como al receptor.

![Comando 6](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%206.png)

El resultado fue la desconexión de ambos dispositivos, lo que evidencia que no cuentan con mecanismos de protección frente a este tipo de ataques.  

- Desconexión del emisor: 

![Desconexion del emisor](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Ca%C3%ADda%20desde%20el%20emisor.png)

- Desconexión del receptor: 

![Desconexion del receptor](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Ca%C3%ADda%20desde%20el%20receptor.png)

En Wireshark se observa la interrupción de la conexión.

![Desconexion en Wireshark](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/ProcesodeDeAuth.png)

Posteriormente, al restablecerse la comunicación, se captura el handshake WPA2 y fue posible confirmar que los paquetes transmitidos por los dispositivos ESP32-WROOM-32 viajaban en **texto plano sobre el puerto TCP 1883 (protocolo MQTT)**.

![Intercambiodehanshakedelosdispositivos](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Intercambiodehandshakedelosdispositivos.png)

Finalmente se analizan los paquetes recopilados y la información extraída de los paquetes. Esta información se presenta directamente en el documento del trabajo de graduación. 

### Comando 7 y 8: 

Finalmente, para detener el modo monitor de la interfaz, se detienen los procesos y se aplican los comandos `sudo systemctl restart NetworkManager` y el comando `sudo airmon-ng stop wlan0mon`, lo que detiene el modo monitor y lo pasa a modo managed. Para comprobarlo, se usa el comando `ip a` y se verifica que la interfaz wlan0mon vuelve a wlan0, recuperando la conexión Wi-Fi. 

![Comando 7-8](https://github.com/Mariyei/TrabMariaAlvarez/blob/main/Resistencia%20ante%20ataques%20Man-in-the-Middle/Esquema%20Sin%20Cifrado/Material%20visual/Comando%207-8.png)















