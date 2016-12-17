#include "./Monitoring.h"

void print_dht(float h, float t, float hic) {
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C");
  Serial.print("\n");
}


MonitoringDHT::MonitoringDHT(uint8_t pin, uint8_t type, char* token, char* host, int port) {
    this->_dht = new DHT(pin, type);
    this->_client = new RestClient(host, port);
    this->_token = token;
}


void MonitoringDHT::begin() {
    this->_dht->begin();
}


void MonitoringDHT::send_datas() {
    String response = "";
    this->_client->setHeader("Content-type: application/json");
    char auth_token[64] = "Authorization: Token ";
    strcat(auth_token, this->_token);
    this->_client->setHeader(auth_token);
    char temperature[6] = "";
    dtostrf(this->_hic, 3, 3, temperature);
    char humidity[8] = "";
    dtostrf(this->_humidity, 3, 3, humidity);
    char datas[128] = "";
    snprintf(datas, 128, "{\"temperature\":\"%s\", \"humidity\":\"%s\", \"owner\":\"tperale\"}", temperature, humidity);
    this->_client->post("/weather/own/", datas, &response);
    Serial.println(response);
}


void MonitoringDHT::do_the_thing() {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    this->_humidity = this->_dht->readHumidity();
    // Read temperature as Celsius (the default)
    this->_temperature = this->_dht->readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(this->_humidity) || isnan(this->_temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Compute heat index in Celsius (isFahreheit = false)
    this->_hic = this->_dht->computeHeatIndex(this->_temperature, this->_humidity, false);

    print_dht(this->_humidity, this->_temperature, this->_hic);

    this->send_datas();
}
