#ifndef BUTTON_ESPIDF_H
#define BUTTON_ESPIDF_H

#include "Button.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"

class ButtonSimpleESPIDF : public ButtonSimple {
public:
    ButtonSimpleESPIDF(uint8_t pin, uint16_t debounce_time, bool pullup = true);
    uint32_t acceptCount_;
    bool pullup_;
    void init() override;
    void update() override;
    bool readButtonState() override;
    static void IRAM_ATTR isrHandler(void* arg);
    static bool isrServiceInstalled_;
protected:
    bool readPin() override;
};

class ButtonFSMESPIDF : public ButtonFSM {
public:
    ButtonFSMESPIDF(uint8_t pin, uint16_t debounce_time,
                    uint16_t short_press_time, uint16_t long_press_time,
                    bool pullup = true);
    bool pullup_;
    void init() override;
    void update() override;
    bool readButtonState() override;
    static void IRAM_ATTR isrHandler(void* arg);
    static bool isrServiceInstalled_;
protected:
    bool readPin() override;
};

#endif // BUTTON_ESPIDF_H
