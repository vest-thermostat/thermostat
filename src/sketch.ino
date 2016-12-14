#include "./settings.h"
#include "./Relay.h"
#include <ESP8266WiFi.h>
// #include <RestClient.h>
// #include <string>
#include <aREST.h>
#include <DHT.h>


/* RestClient client = RestClient("172.23.218.116", 8000); */

// Sensor instance
DHT dht(DHTPIN, DHTTYPE);

// Datas for the API
float temperature = 0;
float humidity = 0;

// Create aREST instance
aREST rest = aREST();

Relay relay = Relay(D4);

// Create an instance of the server
#define LISTEN_PORT 80
WiFiServer server(LISTEN_PORT);

void setup() {
  Serial.begin(9600);

  relay.begin();
  dht.begin();

  rest.variable("temperature", &temperature);
  rest.variable("humidity", &humidity);
  rest.variable("heating", &(relay._state));
  rest.set_name("VEST - Thermostat");

  WiFi.begin(SSID, SSID_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected - "); Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  /* String response = ""; */
  /* client.setHeader("Content-type: application/json"); */
  /* client.get("/", &response); */
  /* Serial.println(response); */

  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temperature, humidity, false);

  print_dht(&humidity, &temperature, &hic);

  delay(500);

  rest.handle(client);
}

void print_dht(float *h, float *t, float* hic) {
  Serial.print("Humidity: ");
  Serial.print(*h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(*t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(*hic);
  Serial.print(" *C");
  Serial.print("\n");
}
