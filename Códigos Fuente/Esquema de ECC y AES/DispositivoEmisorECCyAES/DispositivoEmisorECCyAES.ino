#include "DHT.h"
#include <WiFi.h>
#include "mbedtls/aes.h"
#include "mbedtls/ecdh.h"
#include <PubSubClient.h>
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

const char* ssid = "XXXXXX";           // Nombre de la red WiFi
const char* password = "XXXXXX";       // Contraseña de la red WiFi

const char* mqttServer = "mc93cb1d.ala.dedicated.aws.emqxcloud.com"; // Dirección del broker
const int mqttPort = 1883;                                           // Puerto sin cifrado
const char* mqttUser = "ESP32EMISOR";                                // Usuario del broker
const char* mqttPassword = "esp32sensores";                          // Contraseña del broker
const char* mqttTopic = "datos/dht11";                               // Tema para publicar

//Tópicos para intercambio de claves ECC
const char* TOPIC_EMISOR_PUBKEY = "ecc/emisor_pubkey";
const char* TOPIC_RECEPTOR_PUBKEY = "ecc/receptor_pubkey";

//Contexto de ECC
mbedtls_ecdh_context ctx;
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;

//Variables de ECC
uint8_t my_pubkey[75];
size_t my_pubkey_len = 0;
uint8_t peer_pubkey[75];
size_t peer_pubkey_len = 0;
bool peer_pubkey_received = false;

// Variable para guardar la clave pública anterior
uint8_t last_pubkey_prev[75];
size_t last_pubkey_prev_len = 0;

//Variables para AES
uint8_t aes_key[16]; 

//Contexto de AES
mbedtls_aes_context aes;

#define DHTPIN 13      
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

unsigned long mensajeID = 1; 

//Bandera de claves intercambiadas
bool ClaveObt =  false; 

unsigned long tiempoPrev = 0; 
unsigned long tiempoAct = 0; 
const unsigned long tiempoFin = 120000; 

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
    if (client.connect("ESP32EMISOR", mqttUser, mqttPassword)) { 
      Serial.println("Conectado al broker EMQX.");
      client.subscribe(TOPIC_RECEPTOR_PUBKEY);
      publicar_clave_publica();
    } else {
      Serial.print("Error de conexión: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

// Publica un mensaje vacío en el mismo tópico para eliminar la clave retenida
void eliminar_clave_publica() {
  uint8_t empty_message[] = {};
  client.publish(TOPIC_EMISOR_PUBKEY, empty_message, 0, true);
  Serial.println("Mensaje vacío enviado para eliminar la clave pública retenida.");
}

//Funcion de clave publica publicada
void publicar_clave_publica() {
    eliminar_clave_publica();
    if (client.publish(TOPIC_EMISOR_PUBKEY, my_pubkey, my_pubkey_len, true)) {
    Serial.println("Clave pública del emisor publicada con retain = true.");
  } else {
      Serial.println("Error al publicar la nueva clave pública del receptor.");
    }
}

//Función de padding
int applyPaddingPKCS7(const char* input, int input_len, unsigned char* output) {
  int padding = 16 - (input_len % 16);
  int total_len = input_len + padding;
  memcpy(output, input, input_len);
  // Se añade el padding PKCS#7
  memset(output + input_len, padding, padding);
  return total_len;
}

void pubkey_callback(char* topic, byte* payload, unsigned int length) {
  if (last_pubkey_prev_len == length && memcmp(last_pubkey_prev, payload, length) == 0) {
    Serial.println("Clave pública no ha cambiado, esperando la siguiente...");
    return;
  }
  Serial.println("Clave pública actualizada del receptor.");

  memcpy(last_pubkey_prev, payload, length);
  last_pubkey_prev_len = length;

  memcpy(peer_pubkey, payload, length);
  peer_pubkey_len = length;
  peer_pubkey_received = true;
  
  Serial.print("Clave pública recibida del receptor: ");
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
    for (size_t i = 0; i < 16; i++) Serial.printf("%02X", aes_key[i]);
    Serial.println();

    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, aes_key, 128);

    ClaveObt = true;
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(pubkey_callback);

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
  Serial.print("Mi clave pública (servidor): ");
  for (size_t i = 0; i < my_pubkey_len; i++) Serial.printf("%02X", my_pubkey[i]);
  Serial.println();
  reconnect();
  // Se publicar la clave pública en el topic de emisor
  publicar_clave_publica();

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (!ClaveObt) return;

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
  Serial.println(payload);
  yield();

  // Padding del payload
  int plaintext_len = payload.length();
  unsigned char plaintext[64] = {0};
  int total_len = applyPaddingPKCS7(payload.c_str(), plaintext_len, plaintext);

  // Se cifra el payload con AES-128 ECB
  unsigned char ciphertext[64] = {0};
  for (int i = 0; i < total_len; i += 16) {
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, plaintext + i, ciphertext + i);
  }

  client.publish(mqttTopic, ciphertext, total_len);

  mensajeID++; 
  }
}