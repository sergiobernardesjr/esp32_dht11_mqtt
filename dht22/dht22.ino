#include "DHTesp.h"
#include <PubSubClient.h>
#include <WiFi.h>

#define PUBLISH_DELAY 2000

WiFiClient espClient;         // Cria o objeto espClient
PubSubClient MQTT(espClient); 
const char *SSID = "Bernardes_RX"; // SSID / nome da rede WI-FI que deseja se conectar
const char *PASSWORD = "Bernardes15!"; 
const char *BROKER_MQTT = "10.0.0.12";
const int BROKER_PORT = 1885;
const int DHT_PIN = 21;

static char strTemperature[10] = {0};
unsigned long publishUpdate;

DHTesp dhtSensor;

void initWiFi(void);
void initMQTT(void);
void reconnectMQTT(void);
void reconnectWiFi(void);
void checkWiFIAndMQTT(void);
void callbackMqtt(char *topic, byte *payload, unsigned int length);

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT11);
    // Inicializa a conexao Wi-Fi
  initWiFi();

  // Inicializa a conexao ao broker MQTT
  initMQTT();
}

void callbackMQTT(char *topic, byte *payload, unsigned int length)
{
  String msg;

  // Obtem a string do payload recebido
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    msg += c;
  }

  Serial.printf("Chegou a seguinte string via MQTT: %s do topico: %s\n", msg, topic);
}

void initWiFi(void)
{
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconnectWiFi();
}

void checkWiFIAndMQTT(void)
{
  reconnectWiFi();
  
  if (!MQTT.connected()) {
    Serial.println("Perdeu conexão.");
    reconnectMQTT(); // se não há conexão com o Broker, a conexão é refeita
  }
    

 // se não há conexão com o WiFI, a conexão é refeita
}

void reconnectMQTT(void)
{
  while (!MQTT.connected()) {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    Serial.print(":");
    Serial.println(BROKER_PORT);
    if (MQTT.connect("ID")) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe("comandos");
    } else {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Nova tentativa de conexao em 2 segundos.");
      delay(2000);
    }
  }
}

void reconnectWiFi(void)
{
  // se já está conectado a rede WI-FI, nada é feito.
  // Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

void initMQTT(void)
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(callbackMQTT);
}

void loop() 
{
  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();
    // Verifica o funcionamento das conexões WiFi e ao broker MQTT
    checkWiFIAndMQTT();

    TempAndHumidity data = dhtSensor.getTempAndHumidity();
    Serial.println("Temp: " + String(data.temperature, 2) + "°C");
    Serial.println("Humidity: " + String(data.humidity, 1) + "%");
    sprintf(strTemperature, "C | %.0f", data.temperature);
    MQTT.publish("temperatura", strTemperature);

    // Keep-alive da comunicação com broker MQTT
    MQTT.loop();
  }
}
