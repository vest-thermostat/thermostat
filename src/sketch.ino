#include <ESP8266WiFi.h>
#include <string>
#include <Eventually.h>

#include "./settings.h"
#include "./Relay.h"
#include "./Monitoring.h"

EvtManager mgr;

char* token = "dffc67d2912b21607839301a3121adf1dc41a6ea";
char* host = "192.168.0.208";
int port = 8000;

Relay relay = Relay(D4);

void setup() {
  Serial.begin(9600);

  relay.begin();

  WiFi.begin(SSID, SSID_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected - "); Serial.println(WiFi.localIP());

  mgr.addListener(new MonitoringDHT(1000, DHTPIN, DHTTYPE, token, host, port));

}

USE_EVENTUALLY_LOOP(mgr)
