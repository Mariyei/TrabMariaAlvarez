#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

const char* ssid = "XXXXXX";           // Nombre de la red WiFi
const char* password = "XXXXXX";       // Contraseña de la red WiFi

const char* mqttServer = "mc93cb1d.ala.dedicated.aws.emqxcloud.com"; // Dirección del broker
const int mqttPort = 1883;                                           // Puerto sin cifrado
const char* mqttUser = "ESP32EMISOR";                                // Usuario del broker
const char* mqttPassword = "esp32sensores";                          // Contraseña del broker
const char* mqttTopic = "datos/dht11";                               // Tema para publicar

#define DHTPIN 13        
#define DHTTYPE DHT11    
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long mensajeID = 1; 

unsigned long tiempoPrev = 0; 
unsigned long tiempoAct = 0; 
unsigned long tiempoFin = 120000; 

void setup_wifi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConexión WiFi establecida.");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) { // Usa credenciales
      Serial.println("Conectado al broker EMQX.");
    } else {
      Serial.print("Error de conexión: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  dht.begin(); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  tiempoAct = millis(); 
  if (tiempoAct - tiempoPrev >= tiempoFin) {
    tiempoPrev =  tiempoAct; 
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("Error al leer el sensor DHT11.");
    return;
  }

  String payload = "{\"ID\": " + String(mensajeID) + ", \"temperatura\": " + String(temperatura) + ", \"humedad\": " + String(humedad) + "}";
  client.publish(mqttTopic, payload.c_str());
  Serial.println(payload);

  mensajeID++;  
  }
}