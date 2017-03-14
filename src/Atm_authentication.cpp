#include "Atm_authentication.h"

Atm_authentication& Atm_authentication::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*            ON_ENTER  ON_LOOP  ON_EXIT  EVT_TOKEN  ELSE */
    /* WAITING */       -1,      -1,      -1,   SETTING,   -1,
    /* SETTING */       -1,      -1,      -1,        -1,   -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );

  if (!MDNS.begin("vest")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
    }
  }

  _server.begin();

  _server.on("/", [this] {
    _server.send(200, "text/plain", "Hello resolved by mDNS!");
  });

  _server.on("/setToken", HTTP_POST, [this] {
    String token = _server.arg("token");
    Serial.println(token);
    this->token(token);
  });

  return *this;
}

int Atm_authentication::event( int id ) {
  switch ( id ) {
    case EVT_TOKEN:
      _server.handleClient();
      return 0;
  }
  return 0;
}

void Atm_authentication::action( int id ) {
  switch ( id ) {
  }
}

Atm_authentication& Atm_authentication::trigger( int event ) {
  Machine::trigger( event );
  return *this;
}

int Atm_authentication::state( void ) {
  return Machine::state();
}

Atm_authentication& Atm_authentication::token(String token) {
  if (_callback) {
    _callback(token);
  }
  return *this;
}

Atm_authentication& Atm_authentication::onToken(Auth_Callback c) {
  _callback = c;

  return *this;
}

Atm_authentication& Atm_authentication::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "AUTHENTICATION\0EVT_TOKEN\0ELSE\0WAITING\0SETTING" );
  return *this;
}
