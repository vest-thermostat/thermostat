#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <string>

typedef bool (*Auth_Callback)(char token[]);

unsigned const int defaultPort = 4567;

class Authentication {
    private:
        WiFiUDP Udp;
        char incomingMessageBuffer[255];

    public:
        void sendAck (IPAddress ip, unsigned int port);

        void sendAuthError (IPAddress ip, unsigned int port);

        String waitForAuthentication(Auth_Callback verificationCallback = nullptr, unsigned int port = defaultPort);
};
#endif
