#ifndef BUTTON_ARDUINO_H
#define BUTTON_ARDUINO_H

#include "Button.h"
#include <Arduino.h>

class ButtonSimpleArduino : public ButtonSimple {
public:
    ButtonSimpleArduino(uint8_t pin, uint16_t debounce_time,
                        uint8_t mode = INPUT_PULLUP);
    void init() override;
    void update() override;
    uint8_t mode;
    uint32_t acceptCount_;
    uint32_t lastButtonStateChangeTime_us;
    uint32_t buttonStartWaitAcceptanceTime_us;
    bool readButtonState() override;
    ButtonState state() const { return state_; }
protected:
    bool readPin() override;
    ButtonState state_;
private:
    static void IRAM_ATTR isrHandler(void* arg);
};

class ButtonFSMArduino : public ButtonFSM {
public:
    ButtonFSMArduino(uint8_t pin, uint16_t debounce_time,
                     uint16_t short_press_time, uint16_t long_press_time,
                     uint8_t mode = INPUT_PULLUP);
    void init() override;
    void update() override;
    uint8_t mode;
    bool readButtonState() override;
protected:
    bool readPin() override;
private:
    static void IRAM_ATTR isrHandler(void* arg);
};

#endif // BUTTON_ARDUINO_H
