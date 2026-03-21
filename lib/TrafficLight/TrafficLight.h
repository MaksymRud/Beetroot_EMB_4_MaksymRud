#ifndef TrafficLight_H
#define TrafficLight_H

#include <Arduino.h>
#include <etl/flat_map.h>
#include "Led.h"

enum class TrafficLightState: unsigned long {
    GO,
    READY_BEFORE_STOP,
    STOP_MOVEMENT,
    FULL_STOP,
    READY_BEFORE_GO
};

const etl::flat_map<TrafficLightState, unsigned long, 5> stateDurations = {
    { TrafficLightState::GO,                5000 },
    { TrafficLightState::READY_BEFORE_STOP, 3000 },
    { TrafficLightState::STOP_MOVEMENT,     2000 },
    { TrafficLightState::FULL_STOP,         5000 },
    { TrafficLightState::READY_BEFORE_GO,   2000 }
};

class TrafficLight {
private:
    TrafficLightState state;
    Led redLed;
    Led yellowLed;
    Led greenLed;
    unsigned long lastStateChangeTime;

    void setState(TrafficLightState new_state);
    void setLastStateChangeTime(unsigned long new_last_state_change_time);
    unsigned long getLastStateChangeTime();

public:
    TrafficLight();
    TrafficLight(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin);
    TrafficLight(const Led& red, const Led& yellow, const Led& green);

    TrafficLightState getState();

    void update();
};

#endif