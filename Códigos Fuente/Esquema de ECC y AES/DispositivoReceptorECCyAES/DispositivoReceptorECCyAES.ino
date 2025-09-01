#include <WiFi.h>
#include <Wire.h>
#include "mbedtls/aes.h"
#include <PubSubClient.h>
#include "mbedtls/ecdh.h"
#include "mbedtls/entropy.h"
#include <Adafruit_SSD1306.h>
#include "mbedtls/ctr_drbg.h"

const char* ssid = "XXXXXX";      // Nombre de la red WiFi
const char* password = "XXXXXX";  // Contraseña de la red WiFi

const char* mqttServer = "mc93cb1d.ala.dedicated.aws.emqxcloud.com";  // Dirección del bróker
const int mqttPort = 1883;                                            // Puerto sin cifrado
const char* mqttUser = "ESP32RECEPTOR";                               // Usuario del bróker
const char* mqttPassword = "esp32sensores";                           // Contraseña del bróker
const char* mqttTopic = "datos/dht11";                                // Tema para suscribir

//Tópicos para intercambio de claves ECC
const char* TOPIC_EMISOR_PUBKEY = "ecc/emisor_pubkey";
const char* TOPIC_RECEPTOR_PUBKEY = "ecc/receptor_pubkey";

//Contexto ECC
mbedtls_ecdh_context ctx;
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;

//Variables de ECC
uint8_t my_pubkey[70];
size_t my_pubkey_len = 0;
uint8_t peer_pubkey[70];
size_t peer_pubkey_len = 0;
bool peer_pubkey_received = false;

// Variable para guardar la clave pública anterior
uint8_t last_pubkey_prev[70];
size_t last_pubkey_prev_len = 0;

//Variables de AES
uint8_t aes_key[16];

//Contexto de AES
mbedtls_aes_context aes;

//Bandera de claves intercambiadas
bool ClaveObt =  false; 

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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect("ESP32RECEPTOR", mqttUser, mqttPassword)) {
      Serial.println("Conectado al broker EMQX.");
      client.subscribe(mqttTopic);
      client.subscribe(TOPIC_EMISOR_PUBKEY);
      Serial.println("Suscrito al tema MQTT.");
    } else {
      Serial.print("Error de conexión: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void eliminar_clave_publica() {
  uint8_t empty_message[] = {};
  client.publish(TOPIC_RECEPTOR_PUBKEY, empty_message, 0, true);
  Serial.println("Mensaje vacío enviado para eliminar la clave pública retenida.");
}

void publicar_clave_publica(){
  eliminar_clave_publica();
  if (client.publish(TOPIC_RECEPTOR_PUBKEY, my_pubkey, my_pubkey_len, true)) {
    Serial.println("Nueva clave pública del receptor publicada correctamente.");
  } else {
    Serial.println("Error al publicar la nueva clave pública del receptor.");
  }
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
  display.setTextSize(1); // Texto pequeño
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
if (!ClaveObt) {
    Serial.println("Clave aún no calculada, ignorando mensaje...");
    return;
  }

  // Desencriptacion bloque a bloque
  unsigned char decrypted[64] = {0};
  int blocks = length / 16;

  for (int i = 0; i < blocks; i++) {
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, payload + (i * 16), decrypted + (i * 16));
  }

  // Eliminacion de padding PKCS#7
  int pad_value = decrypted[length - 1];
  int decrypted_len = length - pad_value;
  decrypted[decrypted_len] = '\0';

  Serial.println((char*)decrypted);

  String json = String((char*)decrypted);

  // 1.Extraer ID
  int startId = json.indexOf("\"ID\":") + 5;
  int endId = json.indexOf(",", startId);
  int id = json.substring(startId, endId).toInt();

  // 2.Extraer temperatura
  int startTemp = json.indexOf("temperatura\":") + 13;
  int endTemp = json.indexOf(",", startTemp);
  float temperatura = json.substring(startTemp, endTemp).toFloat();

  // 3.Extraer humedad
  int startHum = json.indexOf("humedad\":") + 9;
  int endHum = json.indexOf("}", startHum);
  float humedad = json.substring(startHum, endHum).toFloat();

  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("Datos inválidos recibidos.");
    return;
  }

  mostrarSoloTempHum(temperatura, humedad);
}

void pubkey_callback(char* topic, byte* payload, unsigned int length) {
  if (last_pubkey_prev_len == length && memcmp(last_pubkey_prev, payload, length) == 0) {
    Serial.println("Clave pública no ha cambiado, esperando la siguiente...");
    return;
  }
  Serial.println("Clave pública actualizada del emisor.");

  memcpy(last_pubkey_prev, payload, length);
  last_pubkey_prev_len = length;

  memcpy(peer_pubkey, payload, length);
  peer_pubkey_len = length;
  peer_pubkey_received = true;
  
  Serial.print("Clave pública recibida del emisor: ");
  for (size_t i = 0; i < length; i++) {
    Serial.printf("%02X", payload[i]);
  }
  Serial.println();

  recalcular_clave_AES();
}

void recalcular_clave_AES() {
  int ret = mbedtls_ecdh_read_public(&ctx, peer_pubkey, peer_pubkey_len);
  if (ret != 0) {
    Serial.println("Error al leer la clave pública");
    return;
  }

  uint8_t shared_secret[32];
  size_t secret_len = 0;
  ret = mbedtls_ecdh_calc_secret(&ctx, &secret_len, shared_secret, sizeof(shared_secret), mbedtls_ctr_drbg_random, &ctr_drbg);
  if (ret != 0) {
    Serial.println("Error al calcular la clave compartida");
    return;
  }

  memcpy(aes_key, shared_secret, 16);
  Serial.print("Clave AES-128 derivada: ");
  for (size_t i = 0; i < 16; i++) {
    Serial.printf("%02X", aes_key[i]);
  }
  Serial.println();
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, aes_key, 128);

  ClaveObt = true;  
}


void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN); 
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(generalCallback);

  // Se inicializa ECDH
  mbedtls_ecdh_init(&ctx);
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  const char *pers = "ecdh";
  int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, strlen(pers));
  if (ret != 0) { Serial.println("DRBG FAIL"); while (1); }
  ret = mbedtls_ecdh_setup(&ctx, MBEDTLS_ECP_DP_SECP256R1);
  if (ret != 0) { Serial.println("ECDH setup FAIL"); while (1); }
  // Se genera el par de claves
  ret = mbedtls_ecdh_make_public(&ctx, &my_pubkey_len, my_pubkey, sizeof(my_pubkey), mbedtls_ctr_drbg_random, &ctr_drbg);
  if (ret != 0) { Serial.println("ECC key FAIL"); while (1); }
  Serial.print("Mi clave pública (cliente): ");
  for (size_t i = 0; i < my_pubkey_len; i++) Serial.printf("%02X", my_pubkey[i]);
  Serial.println();
  reconnect();
  // Se publica la clave pública en el topic de cliente
  publicar_clave_publica();

  if (!display.begin(0x3C)) {
    Serial.println("No se encontró la pantalla OLED.");
    for (;;);
  }
  mostrarTexto("Conectando...");
}

void generalCallback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, TOPIC_EMISOR_PUBKEY) == 0) {
    pubkey_callback(topic, payload, length);
  } else if (strcmp(topic, mqttTopic) == 0) {
    callback(topic, payload, length);
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (!ClaveObt) return;
}