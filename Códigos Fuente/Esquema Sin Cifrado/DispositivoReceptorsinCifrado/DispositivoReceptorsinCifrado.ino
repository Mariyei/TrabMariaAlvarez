#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "XXXXXX";      // Nombre de la red WiFi
const char* password = "XXXXXX";  // Contraseña de la red WiFi

const char* mqttServer = "mc93cb1d.ala.dedicated.aws.emqxcloud.com";  // Dirección del bróker
const int mqttPort = 1883;                                            // Puerto sin cifrado
const char* mqttUser = "ESP32RECEPTOR";                               // Usuario del bróker
const char* mqttPassword = "esp32sensores";                           // Contraseña del bróker
const char* mqttTopic = "datos/dht11";                                // Tema para suscribir

#define SDA_PIN 21
#define SCL_PIN 22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient espClient;
PubSubClient client(espClient);

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

//1. Extraer id
  int startId = json.indexOf("\"ID\":") + 5;
  int endId = json.indexOf(",", startId);
  int id = json.substring(startId, endId).toInt();

//2. Extraer temperatura
  int startTemp = json.indexOf("temperatura\":") + 13;
  int endTemp = json.indexOf(",", startTemp);
  float temperatura = json.substring(startTemp, endTemp).toFloat();

//3. Extraer humedad
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