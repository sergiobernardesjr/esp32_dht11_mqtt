#include "DHTesp.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <stdio.h>
#include <string.h>

#define PUBLISH_DELAY 2000

WiFiClient espClient;         // Cria o objeto espClient
PubSubClient MQTT(espClient); 
const char *SSID = "Bernardes_RX"; // SSID / nome da rede WI-FI que deseja se conectar
const char *PASSWORD = "Bernardes15!"; 
const char *BROKER_MQTT = "10.0.0.12";
const int BROKER_PORT = 1885;
const int DHT_PIN = 22;
const int LED_ON = 18;
const int LED_FAN_1 = 21;
const int LED_FAN_2 = 19;
const int LED_FAN_3 = 23;

static char strParaPublicar[15] = {0};
unsigned long publishUpdate;

DHTesp dhtSensor;

void initWiFi(void);
void initMQTT(void);
void reconnectMQTT(void);
void reconnectWiFi(void);
void checkWiFIAndMQTT(void);
void callbackMqtt(char *topic, byte *payload, unsigned int length);
void temperaturePublish(void);
void humidityPublish(void);
void commandError(void);

void setup() {
  Serial.begin(115200);
 
  dhtSensor.setup(DHT_PIN, DHTesp::DHT11);
  pinMode(LED_FAN_1, OUTPUT);
  pinMode(LED_FAN_2, OUTPUT);
  pinMode(LED_FAN_3, OUTPUT);
  pinMode(LED_ON, OUTPUT);
  
  initWiFi();

  initMQTT();
}

void commandError() {
  MQTT.publish("error", "Comando inválido.");
}

void temperaturePublish() {

  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  sprintf(strParaPublicar, "R|T|%.2f", data.temperature);
  
  MQTT.publish("reading", strParaPublicar);  
}

void humidityPublish() {

  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  sprintf(strParaPublicar, "R|H|%.2f", data.humidity);
  
  MQTT.publish("reading", strParaPublicar);
}

void onOffPublish() {
  
}

void callbackMQTT(char *topic, byte *payload, unsigned int length)
{
  char typeCommand[2] = {0};
  String msgPayload;
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    msgPayload += c;
  }
  
  for (int i = 0; i < 2; i++) {
    typeCommand[i] = (char)payload[i];
  }
  
  if (((typeCommand[0] != 'C') && (typeCommand[0] != 'G') && (typeCommand[0] != 'R') || typeCommand[1] != '|')) {
    commandError(); 
  }
  
  else if (typeCommand[0] == 'R') {
    if (length == 3) {
      char sensorType;
      int indexType = 0;
      for (int i = 2; i < length; i++) {
        sensorType = (char)payload[i];
      }

      if (sensorType == 'T') {
        temperaturePublish();
      } else if (sensorType == 'H') { 
        humidityPublish();
      }
    }
    else
    {
      commandError();
    }
  }
  else if (typeCommand[0] == 'C') {
    if (length == 8) {
      char temperature[2] = {0};
      char modeControl = (char)payload[5];
      char fan = (char)payload[7]; 
      temperature[0] = (char)payload[2];
      temperature[1] = (char)payload[3];
      Serial.println("Temp: " + String(temperature));
      Serial.println("Modo: " + String(modeControl));
      Serial.println("fan: " + String(fan));
      digitalWrite(LED_ON, HIGH);

      if (fan == '1') {
        digitalWrite(LED_FAN_1, HIGH); 
        digitalWrite(LED_FAN_2, LOW);
        digitalWrite(LED_FAN_3, LOW); 
      } 
      else if (fan == '2') {
        digitalWrite(LED_FAN_2, HIGH);  
        digitalWrite(LED_FAN_1, LOW);
        digitalWrite(LED_FAN_3, LOW);
      } 
      else if (fan == '3') {
        digitalWrite(LED_FAN_3, HIGH);  
        digitalWrite(LED_FAN_1, LOW);
        digitalWrite(LED_FAN_2, LOW);
      }

      MQTT.publish("configure", "C|ON");
      
    }
    else {
      commandError();
    }
  }
  else if (typeCommand[0] == 'G') {
    Serial.println("Comando: " + String(typeCommand));
    if (length >= 4 && length <= 5) {
      
      String command;
      for (int i = 2;i < length;i++) {
        char c = (char)payload[i];  
        command += c;
      }
      
      Serial.println("Comando: " + String(msgPayload));
      Serial.println("OFF ou ON: " + String(command));

      if (command.equals("OFF")) {
        digitalWrite(LED_FAN_1, LOW); 
        digitalWrite(LED_FAN_2, LOW);
        digitalWrite(LED_FAN_3, LOW);
        digitalWrite(LED_ON, LOW);
        Serial.println("OFF");
        MQTT.publish("general", "G|OFF");
      }
      else if (command.equals("ON")) {
        digitalWrite(LED_ON, HIGH);
        Serial.println("ON");
        MQTT.publish("general", "G|ON");
      }
    }
    else
    {
      commandError();
    }
  }
  else {
    commandError();
  }
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
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD);

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
    checkWiFIAndMQTT();
    MQTT.loop();
  }
}
