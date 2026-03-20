#ifndef LED_H
#define LED_H

#include <Arduino.h>

enum class LedState {
    OFF,
    ON
};

class Led {
private:
    LedState state;
    uint8_t pin;
    uint16_t blink_delay;
    uint16_t turned_on_delay;
    unsigned long turned_off_time;
    unsigned long turned_on_time;

    void setState(LedState new_state);

    unsigned long getTurnedOnTime();
    void setTurnedOnTime(unsigned long new_turned_on_time);

    unsigned long getTurnedOffTime();
    void setTurnedOffTime(unsigned long new_turned_off_time);

public:
    Led();
    Led(uint8_t pin, uint16_t blink_delay, uint16_t turned_on_delay);

    void on();
    void off();

    LedState getState();

    uint16_t getBlinkDelay();
    void setBlinkDelay(uint16_t new_blink_delay);

    uint16_t getTurnedOnDelay();
    void setTurnedOnDelay(uint16_t new_turned_on_delay);

    void update();
};

#endif
