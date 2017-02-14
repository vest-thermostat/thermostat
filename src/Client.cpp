#include "./Client.h"

VestThermostatClient& VestThermostatClient::begin(const char* host, int port) {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                          ON_ENTER  ON_LOOP  ON_EXIT    EVT_START EVT_NEW_REQUEST  EVT_RESPONSE   EVT_WAIT_REQUEST ELSE */
    /*         IDLE */               -1,      -1,      -1, ENT_WAITING,             -1,           -1,                -1,  -1,
    /*      WAITING */      ENT_WAITING,      -1,      -1,          -1,             -1, NEW_RESPONSE,                -1,  -1,
    /* NEW_RESPONSE */ ENT_NEW_RESPONSE,      -1,      -1,          -1,        WAITING,           -1,            WAITING, -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );

  client = new RestClient(host, port);

  return *this;
}

int VestThermostatClient::event( int id ) {
  switch ( id ) {
    /* case EVT_RESPONSE: */
    /*   return _newResponse; */
    default:
      return 0;
  }
  return 0;
}

void VestThermostatClient::action( int id ) {
  switch ( id ) {
    case ENT_WAITING:
      return;
    case ENT_NEW_RESPONSE:
      if (_callback) {
        _callback(_response);
        trigger(EVT_WAIT_REQUEST);
      }
      return;
  }
}

VestThermostatClient& VestThermostatClient::onResponse(ClientCallback callback) {
  _callback = callback;
  return *this;
}

static String FtoString (float x) {
    char tmp[6] = "";
    dtostrf(x, 3, 3, tmp);
    return String(tmp);
}


VestThermostatClient& VestThermostatClient::sendThermostatDatas (SensorDatas datas) {
    trigger(EVT_NEW_REQUEST);
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

    _response = response;
    trigger(EVT_NEW_REQUEST);

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

VestThermostatClient& VestThermostatClient::addToken(String token) {
    const static char route[] = "/auth/check/";

    _header = String("Authorization: Token ") + token;
    return *this;
}

VestThermostatClient& VestThermostatClient::start() {
  trigger(EVT_START);
  return *this;
}
