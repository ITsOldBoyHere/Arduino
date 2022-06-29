#include <ESP8266WiFi.h>
#include "HX711.h"
#include <PubSubClient.h>
#include <DHT.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>


#define A1   D1
#define A2   D2

int BAT= A0;              //Analog channel A0 as used to measure battery voltage
float RatioFactor=2.1;  //Resistors Ration Factor ...5.714

#define DHTPIN 14          // D5
#define DHTTYPE DHT21     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

HX711 scale;

int  i;
IPAddress staticIP621_10(192,168,2,10);
IPAddress gateway621_10(192,168,2,1);
IPAddress subnet621_10(255,255,255,0);

WiFiClient espClient;
PubSubClient client(espClient);
//MQTT

void reconnectmqttserver() {
while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
String clientId = "ESP8266Client-zibibed-";
clientId += String(random(0xffff), HEX);
if (client.connect(clientId.c_str(), "mqtt", "passs")) {
Serial.println("connected");
Serial.println(clientId);
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");
delay(5000);
}
}
}

char msgmqtt[50];
void callback(char* topic, byte* payload, unsigned int length) {
  String MQTT_DATA = "";
  for (int i=0;i<length;i++) {
   MQTT_DATA += (char)payload[i];}

}




void setup() {
  Serial.begin(9600);
  scale.begin(A1, A2);
  dht.begin();
  float calibration_factor = -7050;

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                // reset the scale to 0

//połączenie z wifi 
  i = 0;
  Serial.begin(9600);
  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
  WiFi.begin("wifi","pass");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("..");

  }
  Serial.println("Connected");
  //WiFi.config(staticIP621_10, gateway621_10, subnet621_10);
  Serial.println("Your IP is");
  Serial.println((WiFi.localIP().toString()));
  client.setServer("ip", 1883);
  client.setCallback(callback);
  
//  timer.setInterval(1000L, sendSensor);
}

void loop() {


  float current_weight=scale.get_units(10);  // get average of 20 scale readings
  float scale_factor=(current_weight/10.40);  // divide the result by a known weight
  Serial.println(scale_factor);

  int value = LOW;
  float Tvoltage=0.0;
  float Vvalue=0.0,Rvalue=0.0;
  for(unsigned int i=0;i<10;i++){
  Vvalue=Vvalue+analogRead(BAT);         //Read analog Voltage
  delay(5);                              //ADC stable
  }
  Vvalue=(float)Vvalue/10.0;            //Find average of 10 values
  Rvalue=(float)(Vvalue/1024.0)*5;      //Convert Voltage in 5v factor
  Tvoltage=Rvalue*RatioFactor;          //Find original voltage by multiplying with factor
    /////////////////////////////////////Battery Voltage//////////////////////////////////
    value = HIGH;
  Serial.println(Tvoltage);


  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
 
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
      delay(2000);
    return;
  }

   Serial.println(h);
   Serial.println(t);
   
if (!client.connected()) {
    reconnectmqttserver();
    }
    client.loop();
    snprintf (msgmqtt, 50, "%f", scale_factor);
    client.publish("waga_zibibed", msgmqtt);
    snprintf (msgmqtt, 50, "%f", Tvoltage);
    client.publish("Bateria", msgmqtt);
    snprintf (msgmqtt, 50, "%f", t);
    client.publish("Temperatura", msgmqtt);
    snprintf (msgmqtt, 50, "%f", h);
    client.publish("Wilgotność", msgmqtt);
    //client.publish("waga", scale_factor);
    i = i + 1;

  scale.power_down();             // put the ADC in sleep mode
  delay(2000);
  scale.power_up();
}
