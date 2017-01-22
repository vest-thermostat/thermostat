#include "Atm_DHT.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

Atm_DHT& Atm_DHT::begin(uint8_t pin, uint8_t type, uint16_t delay) {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                   ON_ENTER  ON_LOOP  ON_EXIT  EVT_INCORRECT_DATAS  EVT_CORRECT_DATAS  EVT_TIMEOUT  EVT_START  ELSE */
    /*      IDLE */            -1,      -1,      -1,                  -1,                -1,          -1,   WAITING,   -1,
    /*   WAITING */   ENT_WAITING,      -1,      -1,                  -1,                -1,      FINISH,        -1,   -1,
    /*    FINISH */    ENT_FINISH,      -1,      -1,             WAITING,         NEW_DATAS,          -1,        -1,   -1,
    /* NEW_DATAS */ ENT_NEW_DATAS,      -1,      -1,                  -1,                -1,          -1,        -1,   -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );

  _pin = pin;
  _type = type;
  _delay = delay;

  return *this;
}

/* Add C++ code for each internally handled event (input)
 * The code must return 1 to trigger the event
 */

int Atm_DHT::event( int id ) {
  switch ( id ) {
    case EVT_INCORRECT_DATAS:
      return 0;
    case EVT_CORRECT_DATAS:
      return 0;
    case EVT_TIMEOUT:
      return mstimer.expired(this);
    case EVT_START:
      return 0;
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 */

void Atm_DHT::action( int id ) {
  switch ( id ) {
    case ENT_WAITING:
      mstimer.set(_delay);
      Serial.print("Delay set to ");
      Serial.println(_delay);
      return;
    case ENT_FINISH:
      Serial.println("FINISH");
      return;
    case ENT_NEW_DATAS:
      return;
  }
}

/* Public event methods
 *
 */

Atm_DHT& Atm_DHT::incorrect_datas() {
  trigger( EVT_INCORRECT_DATAS );
  return *this;
}

Atm_DHT& Atm_DHT::correct_datas() {
  trigger( EVT_CORRECT_DATAS );
  return *this;
}

Atm_DHT& Atm_DHT::timeout() {
  trigger( EVT_TIMEOUT );
  return *this;
}

Atm_DHT& Atm_DHT::start() {
  trigger( EVT_START );
  return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

Atm_DHT& Atm_DHT::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "DHT\0EVT_INCORRECT_DATAS\0EVT_CORRECT_DATAS\0EVT_TIMEOUT\0EVT_START\0ELSE\0IDLE\0WAITING\0FINISH\0NEW_DATAS" );
  return *this;
}
