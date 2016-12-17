#include <ESP8266WiFi.h>
#include <string>
#include <aREST.h>
#include "./settings.h"
#include "./Relay.h"
#include "./Monitoring.h"

MonitoringDHT monitor(DHTPIN, DHTTYPE, "dffc67d2912b21607839301a3121adf1dc41a6ea", "192.168.0.208", 8000);

// Create aREST instance
aREST rest = aREST();

Relay relay = Relay(D4);

// Create an instance of the server
#define LISTEN_PORT 80
WiFiServer server(LISTEN_PORT);

void setup() {
  Serial.begin(9600);

  relay.begin();

  /* rest.variable("temperature", &temperature); */
  /* rest.variable("humidity", &humidity); */
  rest.variable("heating", &(relay._state));
  rest.set_name("VEST - Thermostat");

  WiFi.begin(SSID, SSID_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected - "); Serial.println(WiFi.localIP());

  monitor.begin();

  server.begin();
}

void loop() {
  monitor.do_the_thing();

  delay(500);

  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }

  rest.handle(client);
}
