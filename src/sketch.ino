#include <ESP8266WiFi.h>
#include <string>
#include <Automaton.h>
#include <Atm_esp8266.h>
#include <WiFiManager.h>

#include "./Authentication.h"
#include "./Relay.h"
#include "./Atm_DHT.h"
#include "./Client.h"

#define DHTPIN D5
#define DHTTYPE DHT11

char* host = "192.168.0.208";
int port = 8000;


/* Relay relay = Relay(D4); */

Atm_DHT dht;

void setup() {
  Serial.begin(9600);

  /* relay.begin(); */

  WiFiManager wifiManager;
  wifiManager.autoConnect("VEST Thermostat");

  /* VestThermostatClient client = VestThermostatClient(host, port); */

  /* String token = Authentication().waitForAuthentication(); */

  dht.begin(DHTPIN, DHTTYPE, 5000)
    .onNewDatas([](SensorDatas d) {
      Serial.print("Temperature: ");
      Serial.print(d.temperature);
      Serial.print("Humidity: ");
      Serial.println(d.humidity);
    })
    .start();
}

void loop () {
  automaton.run();
}
