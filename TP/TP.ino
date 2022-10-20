#include <WiFi.h>
#include <MQTT.h>
#include <MQTTClient.h>
#include <SPI.h> // include libraries
#include <LoRa.h>
#define SCK 5 // GPIO5 -- SX127x's SCK
#define MISO 19 // GPIO19 -- SX127x's MISO
#define MOSI 27 // GPIO27 -- SX127x's MOSI
#define SS 18 // GPIO18 -- SX127x's CS
#define RST 14 // GPIO14 -- SX127x's RESET
#define DI0 26 // GPIO26 -- SX127x's IRQ(Interrupt Request)
#define freq 868E6
#define sf 8
#define sb 125E3
const char ssid[] = "Ma";
const char pass[] = "jlmjlmjlm";
WiFiClient net;
MQTTClient client;
WiFiServer server(80);
unsigned long lastMillis = 0;
int etape = 1;

void connect() {
 Serial.print("checking wifi...");
 while (WiFi.status() != WL_CONNECTED) {
 Serial.print("."); delay(1000);
 }
 Serial.print("\nconnecting...");
 while (!client.connect("Jacky")) {
 Serial.print("."); delay(1000);
 }
 client.subscribe("videoprojecteur/reponse");
 client.onMessage(messageReceived);
 Serial.println("\nconnected!");
 server.begin();
}

void messageReceived(String &topic, String &payload) {
 Serial.println("incoming: " + topic + " - " + payload);
 if(payload == "ok"){
    WiFi.softAP("videoprojecteur");
    etape = 2;
 }
}

void setup() {
 Serial.begin(9600);
 WiFi.begin(ssid,pass);
 client.begin("test.mosquitto.org", net);
 connect();
}

void loop() {
 client.loop();
 if (millis() - lastMillis > 3000 && etape == 1) { // publish a message every second.
 lastMillis = millis();
 client.publish("videoprojecteur", WiFi.localIP().toString());
 Serial.println(WiFi.localIP().toString());
 }

 WiFiClient client = server.available();

  if (client){
    while(client.connected()) {
      if(client.available()) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println("{f: 868, sf: 8, sb: 125}");
      }
    }
  }

}
