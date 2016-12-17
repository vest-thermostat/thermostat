#ifndef MONITORING_DHT_H
#define MONITORING_DHT_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#ifdef MONITORING_DEBUG
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

#include "DHT.h"
#include "RestClient.h"
#include <string>
#include <string.h>

class MonitoringDHT {
    private:
        RestClient* _client;
        DHT* _dht;
        char* _token;
        char* _host;
        float _temperature;
        float _humidity;
        float _hic;

    public:
        MonitoringDHT(uint8_t pin, uint8_t type, char* token, char* host, int port);
        void begin(void);
        void send_datas(void);
        void do_the_thing(void);
};
#endif
