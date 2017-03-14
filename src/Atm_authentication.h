#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Automaton.h>

typedef bool (*Auth_Callback)(char token[]);

class Atm_authentication: public Machine {
  public:
    enum { WAITING, SETTING }; // STATES
    enum { EVT_TOKEN, ELSE }; // EVENTS
    Atm_authentication( void ) : Machine() {};
    Atm_authentication& begin( void );
    Atm_authentication& trace( Stream & stream );
    Atm_authentication& trigger( int event );
    int state( void );
    Atm_authentication& token( void );

  private:
    ESP8266WebServer _server = ESP8266WebServer(80);
    enum {  }; // ACTIONS
    int event( int id );
    void action( int id );

};
