#include "Led.h"

void Led::setState(LedState new_state) {
    switch(new_state) {
        case LedState::OFF:
            digitalWrite(pin, LOW);
            state = new_state;
            break;
        case LedState::ON:
            digitalWrite(pin, HIGH);
            state = new_state;
            break;
    };
}

unsigned long Led::getTurnedOnTime() {
    return turned_on_time;
}

void Led::setTurnedOnTime(unsigned long new_turned_on_time) {
    turned_on_time = new_turned_on_time;
}

unsigned long Led::getTurnedOffTime() {
    return turned_off_time;
}

void Led::setTurnedOffTime(unsigned long new_turned_off_time) {
    turned_off_time = new_turned_off_time;
}

Led::Led() : state(LedState::OFF), pin(0), blink_delay(0), turned_off_time(0), turned_on_time(0), turned_on_delay(0) {
    pinMode(0, OUTPUT);
}

Led::Led(uint8_t pin, uint16_t blink_delay, uint16_t turned_on_delay) : state(LedState::OFF), pin(pin), blink_delay(blink_delay), turned_off_time(0), turned_on_time(0), turned_on_delay(turned_on_delay) {
    pinMode(pin, OUTPUT);
}

void Led::on() {
    setState(LedState::ON);
}

void Led::off() {
    setState(LedState::OFF);
}

LedState Led::getState() {
    return state;
}

uint16_t Led::getBlinkDelay() {
    return blink_delay;
}

void Led::setBlinkDelay(uint16_t new_blink_delay) {
    blink_delay = new_blink_delay;
}

uint16_t Led::getTurnedOnDelay() {
    return turned_on_delay;
}

void Led::setTurnedOnDelay(uint16_t new_turned_on_delay) {
    turned_on_delay = new_turned_on_delay;
}

void Led::update() {
    switch (getState())
    {
        case LedState::OFF:
            if (millis() - getTurnedOffTime() >= getBlinkDelay()) {
                on();
                setTurnedOnTime(millis());
                Serial.printf("LED on pin %d turned ON at %lu ms\n", pin, millis());
            }
            break;
        case LedState::ON:
            if (millis() - getTurnedOnTime() >= getTurnedOnDelay()) {
                off();
                setTurnedOffTime(millis());
                Serial.printf("LED on pin %d turned OFF at %lu ms\n", pin, millis());
            }
            break;
    }
}
