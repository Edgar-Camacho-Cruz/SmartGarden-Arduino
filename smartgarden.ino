#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DHTesp.h> 
DHTesp tem;
#define P 16
ESP8266WiFiMulti WiFiMulti;
String login, token;
void setup() {
Serial.begin(115200);

for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("INFINITUM949E", "MU3wa3X7C8");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    inicioSesion();
  tem.setup(P,DHTesp::DHT11);
    if(login == "y"){
    post();
      delay(2000);
    }
  }
  delay(10000);
}

void inicioSesion(){
      WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://192.168.1.143/smarthomerest/login.php?user=admin&pass=123")) {

      Serial.print("[HTTP] GET...\n");
      int httpCode = http.GET();

      if (httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload); 
          int inicio, fin;
          inicio = payload.indexOf("login");
          fin = payload.indexOf(",");
          login =payload.substring(inicio + 8, fin -1);
          Serial.println("login: "+ login);

          inicio = payload.indexOf("token");
          fin = payload.indexOf("}");
          token =payload.substring(inicio + 8, fin -1);
          Serial.println("token: "+ token);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  
  }


void post(){
      WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin(client, "http://192.168.1.143/smarthomerest/sensores.php"); //HTTP
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", token);
    Serial.print("[HTTP] POST...\n");
    int variable=tem.getTemperature();
    int httpCode = http.POST("tipo=T&valor="+ (String)variable);   
    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  
  }
