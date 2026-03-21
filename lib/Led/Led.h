#ifndef LED_H
#define LED_H

#include <Arduino.h>

enum class LedState {
    OFF,
    ON,
    BLINK
};

class Led {
private:
    LedState state;
    uint8_t pin;
    unsigned long blink_delay;
    unsigned long last_switch_time;

    void setState(LedState new_state);

    unsigned long getLastSwitchTime();
    void setLastSwitchTime(unsigned long new_last_switch_time);

public:
    Led();
    Led(uint8_t pin, unsigned long blink_delay);

    void on();
    void off();
    void blink();

    LedState getState();

    unsigned long getBlinkDelay();
    void setBlinkDelay(unsigned long new_blink_delay);

    void update();
};

#endif
