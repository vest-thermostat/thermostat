#include "./Client.h"

VestThermostatClient& VestThermostatClient::begin( const char* host, int port ) {
    Atm_esp8266_httpc_simple::begin(host, port);
    // ADD TOKEN ?
}

static String FtoString (float x) {
    char tmp[6] = "";
    dtostrf(x, 3, 3, tmp);
    return String(tmp);
}

String VestThermostatClient::sendThermostatDatas (SensorDatas datas) {
    const static char route[] = "/weather/own/";

    char query[64];
    snprintf(
        query,
        64,
        "{\"temperature\":\"%s\",\"humidity\":\"%s\"}",
        FtoString(datas.temperature).c_str(),
        FtoString(datas.humidity).c_str()
    );

    this->post(route, query);

    return String();
}

String VestThermostatClient::setTemperature (float temperature) {
    const static char route[] = "/weather/own/set";

    char query[64];
    snprintf(
        query,
        64,
        "{\"temperature\":\"%s\"}",
        FtoString(temperature).c_str()
    );

    this->post(route, query);

    return String();
}
