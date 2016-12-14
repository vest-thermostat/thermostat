#include "./Relay.h"

Relay::Relay(uint8_t pin, int state) {
    this->_pin = pin;
    this->_state = state;
}

void Relay::begin(void) {
    pinMode(this->_pin, OUTPUT);

    DEBUG_PRINTLN("Initialisation of Relay done.")
}

void Relay::set(int state) {
    this->_state = state;
    digitalWrite(this->_pin, this->_state);
}

void Relay::on(void) {
    this->set(HIGH);
}

void Relay::off(void) {
    this->set(LOW);
}

void Relay::toggle(void) {
    this->set(!(this->_state));
}
