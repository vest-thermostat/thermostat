#include <string>
#include <Automaton.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

#include "./Atm_authentication.h"
#include "./Relay.h"
#include "./Atm_DHT.h"
#include "./Client.h"

#define DHTPIN D5
#define DHTTYPE DHT11

const String host = "vest.tperale.be";
const int port = 80;

String token = "349fec562fb9b0bb9e2f8d9b6907b7210e27126a";
/* String token = "968cdbf23d9fc9eaef026e99e05cb11933f0f663"; */

Atm_led relay;

Atm_DHT dht;
VestThermostatClient client;
Atm_authentication auth;

void setup() {
  Serial.begin(9600);

  relay.begin(D4);

  WiFiManager wifiManager;
  wifiManager.autoConnect("VEST Thermostat");

  auth.begin()
    .onToken([](String token) {
      client.addToken(token);
    });

  client.begin("vest.tperale.be", 80)
  /* client.begin("192.168.0.219", 8000) */
    /* .trace(Serial) */
    .addToken(token)
    .onResponse([](String response) {
      String cpy = response;
      int start = 0;
      for (; start < cpy.length(); ++start) {
        if (cpy[start] == '{') {
          break;
        }
      }

      int finish = cpy.length();
      for (; finish < cpy.length(); ++finish) {
         if (cpy[start] == '}') {
          break;
        }
      }

      String json = cpy.substring(start, finish);

      DynamicJsonBuffer jsonBuffer;
      JsonObject& r = jsonBuffer.parseObject(json);
      if (!r.success()) {
        Serial.println("Failed to parse JSON"); 
        return;
      }
      float a = r["humidity"];
      double desired_tmp = r["current_temperature"];
      double temperature = r["temperature"];
      if (desired_tmp >= temperature) {
        Serial.print("ON: "); Serial.print(desired_tmp); Serial.print(" >= "); Serial.println(temperature);
        relay.trigger(relay.EVT_ON);
      } else {
        Serial.print("OFF: "); Serial.print(desired_tmp); Serial.print(" < "); Serial.println(temperature);
        relay.trigger(relay.EVT_OFF);
      }
    }).start();

  dht.begin(DHTPIN, DHTTYPE, 5000)
    /* .trace(Serial) */
    .onNewDatas([](SensorDatas d) {
      client.sendThermostatDatas(d);
    }).start();
}

void loop () {
  automaton.run();
}
