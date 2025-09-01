#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>

const char* ssid = "XXXXXX";      // Nombre de la red WiFi
const char* password = "XXXXXX";  // Contraseña de la red WiFi

const char* mqttServer = "mc93cb1d.ala.dedicated.aws.emqxcloud.com";  // Dirección del bróker
const int   mqttPort = 8883;                                          // Puerto sin cifrado
const char* mqttUser = "ESP32RECEPTOR";                               // Usuario del bróker
const char* mqttPassword = "esp32sensores";                           // Contraseña del bróker
const char* mqttTopic = "datos/dht11";                                // Tema para suscribir

#define SDA_PIN 21
#define SCL_PIN 22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración del cliente MQTT en modo seguro
WiFiClientSecure espClient;
PubSubClient client(espClient);

//Certificado del bróker 
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

void setup_wifi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConexión WiFi establecida.");
}

void mostrarTexto(const char* text) {
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 12); 
  display.println(text);
  display.display();
}

void mostrarSoloTempHum(float temperatura, float humedad) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 8);
  display.print("Temp: ");
  display.print(temperatura, 1);
  display.println(" C");
  display.setCursor(0, 20);
  display.print("Hum:  ");
  display.print(humedad, 1);
  display.println(" %");
  display.display();
}

void callback(char* topic, byte* payload, unsigned int length) {
  char mensaje[length + 1];
  memcpy(mensaje, payload, length);
  mensaje[length] = '\0';
  Serial.println(mensaje);

  String json = String(mensaje);

  // 1. Extraer id
  int startId = json.indexOf("\"ID\":") + 5;
  int endId = json.indexOf(",", startId);
  int id = json.substring(startId, endId).toInt();

  // 2. Extraer temperatura
  int startTemp = json.indexOf("temperatura\":") + 13;
  int endTemp = json.indexOf(",", startTemp);
  float temperatura = json.substring(startTemp, endTemp).toFloat();

  // 3. Extraer humedad
  int startHum = json.indexOf("humedad\":") + 9;
  int endHum = json.indexOf("}", startHum);
  float humedad = json.substring(startHum, endHum).toFloat();

  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("Datos inválidos recibidos.");
    return;
  }

  mostrarSoloTempHum(temperatura, humedad);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect("ESP32Receiver", mqttUser, mqttPassword)) {
      Serial.println("Conectado al broker EMQX.");
      client.subscribe(mqttTopic, 1);
      Serial.println("Suscrito al tema MQTT.");
    } else {
      Serial.print("Error de conexión: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN); 
  setup_wifi();
  espClient.setCACert(root_ca);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  if (!display.begin(0x3C)) {
    Serial.println("No se encontró la pantalla OLED.");
    for (;;);
  }
  mostrarTexto("Conectando...");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}