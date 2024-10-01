#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define BUFFER_SIZE 10

const char *ssid = "<NOME DA TUA REDE>";
const char *password = "<SENHA DA TUA REDE>";
const char *mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// Global Variables
const int pinLed = D6;
const int pinDevice = D7;

float luminosity = 499.0;
float temperature = 100.0;

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void turnOnLed()
{
  digitalWrite(pinLed, HIGH);
}

void turnOffLed()
{
  digitalWrite(pinLed, LOW);
}

void turnOnDevice()
{
  // TEMP: remove when using another output actuator
  digitalWrite(pinDevice, HIGH);
}

void turnOffDevice()
{
  // TEMP: remove when using another output actuator
  digitalWrite(pinDevice, LOW);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String topicAsString = String(topic);
  String command;
  for (int i = 0; i < length; i++)
  {
    command += (char)payload[i];
  }

  if (topicAsString == "/sihs/equipe-gab-nando/1/luz" && command == "ligar")
  {
    Serial.println("Ligando a luz.");
    turnOnLed();
  }
  else if (topicAsString == "/sihs/equipe-gab-nando/1/luz" && command == "desligar")
  {
    Serial.println("Desligando a luz.");
    turnOffLed();
  }
  else if (topicAsString == "/sihs/equipe-gab-nando/2/ac" && command == "ligar")
  {
    Serial.println("Ligando ar-condicionado.");
    turnOnDevice();
  }
  else if (topicAsString == "/sihs/equipe-gab-nando/2/ac" && command == "desligar")
  {
    Serial.println("Desligando ar-condicionado.");
    turnOffDevice();
  }
}

void subscribe()
{
  client.subscribe("/sihs/equipe-gab-nando/1/luz");
  client.subscribe("/sihs/equipe-gab-nando/2/ac");
  // client.subscribe("/sihs/equipe-gab-nando/3/ac");
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      subscribe();
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(pinLed, OUTPUT);
  pinMode(pinDevice, OUTPUT);
}

void loop()
{
  char buffer[BUFFER_SIZE];

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  dtostrf(luminosity, BUFFER_SIZE, 3, buffer);
  client.publish("/sihs/equipe-gab-nando/1/lum", buffer);

  dtostrf(temperature, BUFFER_SIZE, 3, buffer);
  client.publish("/sihs/equipe-gab-nando/2/temp", buffer);

  // client.publish("/sihs/equipe-gab-nando/3/temp", buffer);
}