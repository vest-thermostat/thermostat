#ifndef RELAY_H
#define RELAY_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#ifdef RELAY_DEBUG
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

class Relay {
    private:
        uint8_t _pin;

    public:
        int _state;

        Relay(uint8_t pin, int state=LOW);
        void begin(void);
        void set(int state);
        void on(void);
        void off(void);
        void toggle(void);
};
#endif
