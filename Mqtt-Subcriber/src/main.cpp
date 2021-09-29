#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// University
// #define wifi_ssid "BrijeshWiFi"
// #define wifi_password "IoTlab32768"
// #define mqtt_server "192.168.50.54"
// #define mqtt_user "vrel"
// #define mqtt_password "vrel2021"

// Home 
#define wifi_ssid "Kujawska"
#define wifi_password "17@brijesh"
#define mqtt_server "157.158.56.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"

// MQTT messages (change it to "BrijeshSUB1")
#define MQTTClientName "BrijeshSUB"
#define servoTopic "/vrel/brijes/+/+/#"
#define servoTopic1 "/vrel/brijesh/temp/1"
#define servoTopic2 "/vrel/brijesh/temp/2"
#define topicForSave "vrel/brijesh/save"



//MQTT last will
#define lastWillTopic "/sut/aeii/iot-open.eu/vrel/node9/state"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"

// WiFi & MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//Time Variable
const byte interruptPin = D2;
int state = 0;
bool flag = true;
bool flagInterrupt = false;
bool flag3 = false;


unsigned long startTimeMicros = 0;
unsigned long endTimeMicros = 0;
unsigned long deltaTime = 0;
int n;

char buffer[30];
char buffer2[30];

char bufferSave1[100];
char bufferSave2[6000];

StaticJsonDocument<6000> doc;
char JSONBuffer[6000];
bool flag12 = true;
int count12 = 0;
JsonArray ResultValues = doc.createNestedArray("Results");


void setup_wifi() {
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  n=0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
}

void reconnect()
{
  while (!client.connected()) {
    if (client.connect(MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage)) {
      client.subscribe(servoTopic1);
      Serial.print("subcribed..");
    } else {
      delay(5000);
    }
  } 
}

void mqttPublish()
{   
  if (client.connected())
  {
    delay(10);
    sprintf(bufferSave1, "%s" ,topicForSave);
    sprintf(bufferSave2, "%s" , JSONBuffer);
    client.publish(bufferSave1, bufferSave2, false);
  }
}
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  
  if (strcmp(topic,servoTopic1)==0 && startTimeMicros > 0 && endTimeMicros == 0 && flagInterrupt == true){

    endTimeMicros = millis();
    deltaTime = endTimeMicros - startTimeMicros;
    Serial.print("time: ");
    Serial.println(deltaTime);

    ResultValues.add(deltaTime);
    // serializeJson(doc, Serial);

    startTimeMicros = 0;
    endTimeMicros = 0;
    flagInterrupt = false;
    Serial.println("---"); 
    count12++;
  }

  if(strcmp(topic,servoTopic2)==0){
    Serial.println("here");
  }
  
  if(count12 == 50){
    serializeJson(doc, JSONBuffer);
    Serial.println();
    serializeJson(doc, Serial);
    mqttPublish();
    Serial.println("Published");
    ResultValues.clear();
    doc.garbageCollect();

    count12 = 0;
  }
}

void IRAM_ATTR interruptHanddler(){
  startTimeMicros = millis();
  Serial.print("start...");
  flagInterrupt = true;
  endTimeMicros = 0;
}

void setup()
{
  delay(10);
  Serial.begin(9600);
  setup_wifi();
  Serial.println("WIFI connected");
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);

  // Interrupt
  delay(1000);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(interruptPin,interruptHanddler, CHANGE);
}

void loop()
{ 
  if (!client.connected()){
    reconnect();
  }
  client.loop();
}
