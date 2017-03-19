#ifndef ATM_DHT_H
#define ATM_DHT_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Automaton.h>
#include <string>
#include <string.h>

#ifdef DHT_DEBUG
  #define DEBUG_PRINT(...) { Serial.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { Serial.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

// Define types of sensors.
#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

typedef struct {
  float temperature;
  float humidity;
  float heat_index;
} SensorDatas;

typedef void (*DHTCallback)(SensorDatas);

class Atm_DHT: public Machine {
  private:
    class InterruptLock {
      public:
        InterruptLock() {
          noInterrupts();
        }

        ~InterruptLock() {
          interrupts();
        }
    };

    // BASIC DATA FOR CONFIGURATION
    uint8_t _pin;
    uint8_t _type;
    uint16_t _delay;

    // AUTOMATON DATAS
    enum { ENT_WAITING, ENT_FINISH, ENT_NEW_DATAS }; // ACTIONS
    int event( int id );
    void action( int id );
    atm_timer_millis mstimer;
    DHTCallback _callback;

    // DATA FROM THE SENSOR
    uint8_t _rawdata[5];
    uint32_t _maxcycles = microsecondsToClockCycles(1000);
    SensorDatas _datas;

    uint32_t expectPulse(bool);
    bool _read ();
    float getTemperature();
    float getHumidity();
    float getHeatIndex(float, float);

  public:
    enum { IDLE, WAITING, FINISH, NEW_DATAS }; // STATES
    enum { EVT_INCORRECT_DATAS, EVT_CORRECT_DATAS, EVT_TIMEOUT, EVT_START, ELSE }; // EVENTS
    Atm_DHT(void): Machine() {
      _maxcycles = microsecondsToClockCycles(1000);
      // 1 millisecond timeout for
      // reading pulses from DHT sensor.
      // Note that count is now ignored as the DHT reading algorithm adjusts itself
      // basd on the speed of the processor.
    };
    Atm_DHT& begin(uint8_t pin, uint8_t type, uint16_t delay);
    Atm_DHT& trace( Stream & stream );
    Atm_DHT& incorrect_datas( void );
    Atm_DHT& correct_datas( void );
    Atm_DHT& timeout( void );
    Atm_DHT& start( void );
    Atm_DHT& onNewDatas(DHTCallback);
};
#endif
