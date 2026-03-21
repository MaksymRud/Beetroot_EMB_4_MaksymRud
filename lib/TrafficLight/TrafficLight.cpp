#include "TrafficLight.h"

TrafficLight::TrafficLight() :
    state(TrafficLightState::FULL_STOP),
    redLed(Led()),
    yellowLed(Led()),
    greenLed(Led()),
    lastStateChangeTime(0) {
        yellowLed.off();
        redLed.on();
        greenLed.off();
    }

TrafficLight::TrafficLight(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin) :
    state(TrafficLightState::FULL_STOP),
    redLed(Led(redPin, 500)),
    yellowLed(Led(yellowPin, 500)),
    greenLed(Led(greenPin, 200)),
    lastStateChangeTime(0) {
        yellowLed.off();
        redLed.on();
        greenLed.off();
    }

TrafficLight::TrafficLight(const Led& red, const Led& yellow, const Led& green) :
    state(TrafficLightState::FULL_STOP),
    redLed(red),
    yellowLed(yellow),
    greenLed(green),
    lastStateChangeTime(0) {
        yellowLed.off();
        redLed.on();
        greenLed.off();
    }

void TrafficLight::setLastStateChangeTime(unsigned long new_last_state_change_time) {
    lastStateChangeTime = new_last_state_change_time;
}

unsigned long TrafficLight::getLastStateChangeTime() {
    return lastStateChangeTime;
}

void TrafficLight::setState(TrafficLightState new_state) {
    state = new_state;
}

TrafficLightState TrafficLight::getState() {
    return state;
}

void TrafficLight::update() {
    if (millis() - getLastStateChangeTime() >= stateDurations.at(state)) {
        switch(state) {
            case TrafficLightState::GO:
                greenLed.blink();
                yellowLed.off();
                redLed.off();
                setState(TrafficLightState::READY_BEFORE_STOP);
                break;
            case TrafficLightState::READY_BEFORE_STOP:
                greenLed.off();
                yellowLed.on();
                redLed.off();
                setState(TrafficLightState::STOP_MOVEMENT);
                break;
            case TrafficLightState::STOP_MOVEMENT:
                greenLed.off();
                yellowLed.off();
                redLed.on();
                setState(TrafficLightState::FULL_STOP);
                break;
            case TrafficLightState::FULL_STOP:
                greenLed.off();
                yellowLed.on();
                redLed.on();
                setState(TrafficLightState::READY_BEFORE_GO);
                break;
            case TrafficLightState::READY_BEFORE_GO:
                greenLed.on();
                yellowLed.off();
                redLed.off();
                setState(TrafficLightState::GO);
                break;
        }
        setLastStateChangeTime(millis());
    }
    greenLed.update();
    yellowLed.update();
    redLed.update();
}