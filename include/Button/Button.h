#ifndef BUTTON_ESPIDF_H
#define BUTTON_ESPIDF_H

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"

#include <stdint.h>

enum class ButtonState : uint8_t {
    Idle,
    Pressed,
    Held,
    Released
};

class Button {
public:
    static void IRAM_ATTR isrHandler(void* arg);
    static bool isr_service_installed_;
    bool press_registered = false;

    Button(uint8_t pin, uint16_t debounce_time,
           uint16_t short_press_time, uint16_t long_press_time,
           bool pullup = true);

    void update();


protected:
    const uint8_t pin_;
    volatile bool interruptFlag_;
    uint16_t debounce_time_;
    uint32_t last_change_time_;
    uint16_t short_press_time_;
    uint16_t long_press_time_;
    ButtonState state_;
    const bool pullup_;
};

#endif // BUTTON_ESPIDF_H
