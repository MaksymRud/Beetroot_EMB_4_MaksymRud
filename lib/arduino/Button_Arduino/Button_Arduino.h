#ifndef BUTTON_ARDUINO_H
#define BUTTON_ARDUINO_H

#include "Button.h"
#include <Arduino.h>

class ButtonSimpleArduino : public ButtonSimple {
public:
    ButtonSimpleArduino(uint8_t pin, uint16_t debounce_time,
                        uint8_t mode = INPUT_PULLUP)
        : ButtonSimple(pin, debounce_time),
          inputMode(mode) {}
    void init() override {
        pinMode(pin_, inputMode);
        attachInterruptArg(digitalPinToInterrupt(pin_), isrHandler, this, FALLING);
    }
    uint8_t inputMode;
    bool readButtonState() override {
        return (inputMode == INPUT_PULLUP) ? !readPin() : readPin();
    }
protected:
    bool readPin() override { return digitalRead(pin_); }
private:
    static void IRAM_ATTR isrHandler(void* arg);
};

class ButtonFSMArduino : public ButtonFSM {
public:
    ButtonFSMArduino(uint8_t pin, uint16_t debounce_time,
                     uint16_t short_press_time, uint16_t long_press_time,
                     uint8_t mode = INPUT_PULLUP)
        : ButtonFSM(pin, debounce_time, short_press_time, long_press_time),
          inputMode(mode) {}
    void init() override {
        pinMode(pin_, inputMode);
        attachInterruptArg(digitalPinToInterrupt(pin_), isrHandler, this, FALLING);
    }
    uint8_t inputMode;
    bool readButtonState() override {
        return (inputMode == INPUT_PULLUP) ? !readPin() : readPin();
    }
protected:
    bool readPin() override { return digitalRead(pin_); }
private:
    static void IRAM_ATTR isrHandler(void* arg);
};

#endif // BUTTON_ARDUINO_H
