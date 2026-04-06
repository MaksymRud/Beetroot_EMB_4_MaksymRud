#ifndef BUTTON_ESPIDF_H
#define BUTTON_ESPIDF_H

#include "Button.h"
#include "driver/gpio.h"
#include "esp_timer.h"

class ButtonSimpleESPIDF : public ButtonSimple {
public:
    ButtonSimpleESPIDF(uint8_t pin, uint16_t debounce_time, bool pullup = false)
        : ButtonSimple(pin, debounce_time), pullup_(pullup) {}
    void init() override;
    bool readButtonState() override {
        return pullup_ ? !readPin() : readPin();
    }
    static void IRAM_ATTR isrHandler(void* arg);
    static bool isrServiceInstalled_;
    bool pullup_;
protected:
    bool readPin() override { return gpio_get_level((gpio_num_t)pin_) == 1; }
};

class ButtonFSMESPIDF : public ButtonFSM {
public:
    ButtonFSMESPIDF(uint8_t pin, uint16_t debounce_time,
                    uint16_t short_press_time, uint16_t long_press_time,
                    bool pullup = false)
        : ButtonFSM(pin, debounce_time, short_press_time, long_press_time),
          pullup_(pullup) {}
    void init() override;
    bool readButtonState() override {
        return pullup_ ? !readPin() : readPin();
    }
    static void IRAM_ATTR isrHandler(void* arg);
    static bool isrServiceInstalled_;
    bool pullup_;
protected:
    bool readPin() override { return gpio_get_level((gpio_num_t)pin_) == 1; }
};

#endif // BUTTON_ESPIDF_H
