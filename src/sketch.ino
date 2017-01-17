#include <ESP8266WiFi.h>
#include <string>
#include <Automaton.h>
#include <Atm_esp8266.h>
#include <WiFiManager.h>

#include "./settings.h"
#include "./Relay.h"
#include "./Monitoring.h"

char* host = "192.168.0.208";
int port = 8000;

DHT dht = DHT(DHTPIN, DHTTYPE);

/* Relay relay = Relay(D4); */

Atm_timer timer;

void setup() {
  Serial.begin(9600);

  /* relay.begin(); */
  /* mgr.addListener(new MonitoringDHT(1000, DHTPIN, DHTTYPE, token, host, port)); */

  WiFiManager wifiManager;
  wifiManager.autoConnect("VEST Thermostat");

  dht.begin();

  timer.begin(1000)
    .repeat()
    .onTimer([]( int idx, int v, int up ) {
      Serial.print(dht.readHumidity());
    }).start();
}

void loop () {
  automaton.run();
}
