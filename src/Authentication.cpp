#include "./Authentication.h"

void Authentication::sendAck (IPAddress ip, unsigned int port) {
    Udp.beginPacket(ip, port);
    Udp.write("Ack");
    Udp.endPacket();
}

void Authentication::sendAuthError (IPAddress ip, unsigned int port) {
    Udp.beginPacket(ip, port);
    Udp.write("Error");
    Udp.endPacket();
}

String Authentication::waitForAuthentication(Auth_Callback verificationCallback, unsigned int port) {
    Udp.begin(port);

    while (1) {
        int packetSize = Udp.parsePacket();
        if (packetSize) {
            IPAddress senderIP = Udp.remoteIP();
            unsigned const int senderPort = Udp.remotePort();

            int len = Udp.read(incomingMessageBuffer, 255);
            if (len > 0) {
                incomingMessageBuffer[len] = '\0';
            }

            if (verificationCallback) {
                if ((*verificationCallback)(incomingMessageBuffer)) {
                    sendAck(senderIP, senderPort);
                } else {
                    sendAuthError(senderIP, senderPort);
                }
            } else {
                sendAck(senderIP, senderPort);
            }
        }
    }

    Udp.stop();

    return String(incomingMessageBuffer);
}
