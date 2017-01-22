#include "./Client.h"

VestThermostatClient& VestThermostatClient::begin(const char* host, int port) {
    client = new RestClient(host, port);

    return *this;
}

static String FtoString (float x) {
    char tmp[6] = "";
    dtostrf(x, 3, 3, tmp);
    return String(tmp);
}

VestThermostatClient& VestThermostatClient::sendThermostatDatas (SensorDatas datas) {
    const static char route[] = "/weather/own/";

    char query[64];
    snprintf(
        query,
        64,
        "{\"temperature\":\"%s\",\"humidity\":\"%s\",\"owner\":\"thomas\"}",
        FtoString(datas.temperature).c_str(),
        FtoString(datas.humidity).c_str()
    );

    Serial.println(query);

    String response;

    /* client->setHeader("Accept: application/json"); */
    client->setHeader(_header.c_str());
    client->setContentType("application/json");
    client->post(route, query, &response);

    /* Serial.println(response); */

    return *this;
}

VestThermostatClient& VestThermostatClient::setTemperature (float temperature) {
    const static char route[] = "/weather/own/set";

    char query[64];
    snprintf(
        query,
        64,
        "{\"temperature\":\"%s\"}",
        FtoString(temperature).c_str()
    );

    client->post(route, query);

    return *this;
}

bool VestThermostatClient::addToken(String token) {
    const static char route[] = "/auth/check/";

    _header = String("Authorization: Token ") + token;
}
