#ifndef MONITORING_DHT_H
#define MONITORING_DHT_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#ifdef MONITORING_DEBUG
  #define DEBUG_PRINT(...) { Serial.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { Serial.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

#include <DHT.h>
#include <RestClient.h>
#include <Eventually.h>
#include <string>
#include <string.h>

class MonitoringDHT {
    private:
        EvtManager* _mgr;
        RestClient* _client;
        DHT* _dht;
        char* _token;
        char* _host;
        float _temperature;
        float _humidity;
        float _hic;

    public:
        MonitoringDHT(EvtManager* mgr,  uint8_t pin, uint8_t type, char* token, char* host, int port);
        void begin(void);
        void send_datas(void);
        bool do_the_thing(EvtListener*, EvtContext*);
};
#endif
