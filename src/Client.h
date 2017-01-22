#ifndef VEST_CLIENT_H
#define VEST_CLIENT_H

#include <RestClient.h>
#include <string>
#include "./Atm_DHT.h"

/* @desc: Wrapper to communicate to the VEST Thermostat server API.
 */
class VestThermostatClient {
    private:
        String _header;
        RestClient* client;

    public:
        VestThermostatClient() {};
        VestThermostatClient& begin (const char* host, int port = 80);
        VestThermostatClient& sendThermostatDatas (SensorDatas);
        VestThermostatClient& setTemperature (float temperature);
        bool addToken (String token);
};

#endif
