#ifndef VEST_CLIENT_H
#define VEST_CLIENT_H

#include <Automaton.h>
#include <RestClient.h>
#include <string>
#include <ArduinoJson.h>
#include "./Atm_DHT.h"

typedef void (*ClientCallback)(String);

/* @desc: Wrapper to communicate to the VEST Thermostat server API.
 */
class VestThermostatClient: public Machine {
    private:
        String _header;
        /* bool _newResponse; */
        String _response;
        RestClient* client;

        enum { ENT_WAITING, ENT_NEW_RESPONSE };
        int event(int id);
        void action(int id);
        ClientCallback _callback;

    public:
        enum { IDLE, WAITING, NEW_RESPONSE };
        enum { EVT_START, EVT_NEW_REQUEST, EVT_RESPONSE, EVT_WAIT_REQUEST, ELSE };
        VestThermostatClient(): Machine() {};
        VestThermostatClient& begin (const char* host, int port = 80);
        VestThermostatClient& sendThermostatDatas (SensorDatas);
        VestThermostatClient& setTemperature (float temperature);
        VestThermostatClient& addToken (String token);
        VestThermostatClient& onResponse(ClientCallback);
        VestThermostatClient& start();
};

#endif
