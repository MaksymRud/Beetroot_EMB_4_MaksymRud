#include "Led.h"

void Led::setState(LedState new_state) {
    switch(new_state) {
        case LedState::OFF:
            digitalWrite(pin, LOW);
            break;
        case LedState::ON:
            digitalWrite(pin, HIGH);
            break;
    };
    state = new_state;
    setLastSwitchTime(millis());
}

unsigned long Led::getLastSwitchTime() {
    return last_switch_time;
}

void Led::setLastSwitchTime(unsigned long new_last_switch_time) {
    last_switch_time = new_last_switch_time;
}

Led::Led() :
    state(LedState::OFF),
    pin(10),
    blink_delay(0),
    last_switch_time(0) {
    pinMode(pin, OUTPUT);
}

Led::Led(uint8_t pin, unsigned long blink_delay) :
    state(LedState::OFF),
    pin(pin),
    blink_delay(blink_delay),
    last_switch_time(0) {
    pinMode(pin, OUTPUT);
}

void Led::on() {
    setState(LedState::ON);
}

void Led::off() {
    setState(LedState::OFF);
}

void Led::blink() {
    setState(LedState::BLINK);
}

LedState Led::getState() {
    return state;
}

unsigned long Led::getBlinkDelay() {
    return blink_delay;
}

void Led::setBlinkDelay(unsigned long new_blink_delay) {
    blink_delay = new_blink_delay;
}

void Led::update() {
    if (state == LedState::BLINK) {
        if (millis() - getLastSwitchTime() >= getBlinkDelay()) {
            if (digitalRead(pin) == LOW) {
                digitalWrite(pin, HIGH);
                setLastSwitchTime(millis());
            } else {
                digitalWrite(pin, LOW);
                setLastSwitchTime(millis());
            }
        }
    }
}
