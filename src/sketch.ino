#include <string>
#include <Automaton.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

#include "./Authentication.h"
#include "./Relay.h"
#include "./Atm_DHT.h"
#include "./Client.h"

#define DHTPIN D5
#define DHTTYPE DHT11

const String host = "192.168.0.219";
const int port = 8000;

String token = "b54f5752ff9ae4a3d8af05fc1cbe6cb33390c65a";

/* Relay relay = Relay(D4); */

Atm_DHT dht;
VestThermostatClient client;
StaticJsonBuffer<200> jsonBuffer;

void setup() {
  Serial.begin(9600);

  /* relay.begin(); */

  WiFiManager wifiManager;
  wifiManager.autoConnect("VEST Thermostat");

  client.begin("192.168.0.102", 8000)
    .addToken(token)
    .onResponse([](String response) {
      JsonObject& r = jsonBuffer.parseObject(response);
      double desired_tmp = r["current_temperature"];
      double temperature = r["temperature"];
      if (desired_tmp >= temperature) {
        /* relay.trigger(ON); */
      } else {
        /* relay.trigger(OFF); */
      }
    }).start();

  /* String token = Authentication().waitForAuthentication(); */

  dht.begin(DHTPIN, DHTTYPE, 2000)
    /* .trace(Serial) */
    .onNewDatas([](SensorDatas d) {
      client.sendThermostatDatas(d);
    }).start();
}

void loop () {
  automaton.run();
}
