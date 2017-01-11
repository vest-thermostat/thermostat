#ifndef VEST_CLIENT_H
#define VEST_CLIENT_H

#include <Atm_esp8266.h>
#include <string>
#include "Monitoring.h"

/* @desc:
 */
class VestThermostatClient: Atm_esp8266_httpc_simple {
    private:
    public:
        VestThermostatClient& begin( const char* host, int port = 80 );
        String sendThermostatDatas (SensorDatas);
        String setTemperature (float temperature);
};

#endif
