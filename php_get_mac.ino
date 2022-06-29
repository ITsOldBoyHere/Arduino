#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "wifi";  
const char *password = "pass";

const char *host = "192.168.2.243";   
void setup() {
  delay(1000);
  Serial.begin(9600);
  WiFi.mode(WIFI_OFF);        
  delay(1000);
  WiFi.mode(WIFI_STA);          
  WiFi.begin(ssid, password);   
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 //String mac_adress = WiFi.macAddress();

   HTTPClient http;    

  String ADCData, station, getData, Link;
  int adcvalue=analogRead(A0);
  ADCData = String(adcvalue);
  station = "B";
  String mac_adress = WiFi.macAddress();

  getData = "?macadres=" + mac_adress;
  Link = "http://192.168.2.243/esp.php" + getData;
  
  http.begin(Link);     //Specify request destination
  
  int httpCode = http.GET();
  String payload = http.getString();

  Serial.println(httpCode);
  Serial.println(payload); 

  http.end();
  
  delay(5000);
}

void loop() {

}
//=======================================================================
