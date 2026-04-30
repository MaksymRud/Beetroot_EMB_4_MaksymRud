#include "Button.h"

bool Button::isr_service_installed_ = false;

Button::Button(uint8_t pin, uint16_t debounce_time,
               uint16_t short_press_time, uint16_t long_press_time,
               bool pullup)
    : pin_(pin), debounce_time_(debounce_time), last_change_time_(0),
      short_press_time_(short_press_time), long_press_time_(long_press_time),
      state_(ButtonState::Idle), pullup_(pullup),
      interruptFlag_(false)
{
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = (1ULL << pin_);
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = pullup_ ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = pullup_ ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE;
    cfg.intr_type = pullup_ ? GPIO_INTR_NEGEDGE : GPIO_INTR_POSEDGE;
    gpio_config(&cfg);

    if (!isr_service_installed_) {
        gpio_install_isr_service(0);
        isr_service_installed_ = true;
    }
    gpio_isr_handler_add((gpio_num_t)pin_, isrHandler, this);
}

void Button::update() {
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
    bool pressed = pullup_ ? (gpio_get_level((gpio_num_t)pin_) == 0)
                           : (gpio_get_level((gpio_num_t)pin_) == 1);
    if (interruptFlag_) {
        switch (state_) {
            case ButtonState::Idle:
                if (pressed) {
                    state_ = ButtonState::Pressed;
                    last_change_time_ = current_time;
                }
                break;

            case ButtonState::Pressed:
                if (!pressed) {
                    state_ = ButtonState::Released;
                    last_change_time_ = current_time;
                } else if (current_time - last_change_time_ >= long_press_time_) {
                    state_ = ButtonState::Held;
                }
                break;

            case ButtonState::Held:
                if (!pressed) {
                    state_ = ButtonState::Released;
                    last_change_time_ = current_time;
                }
                break;

            case ButtonState::Released:
                if (pressed) {
                    state_ = ButtonState::Pressed;
                    last_change_time_ = current_time;
                    interruptFlag_ = false;
                    press_registered = true;
                } else if (current_time - last_change_time_ >= debounce_time_) {
                    state_ = ButtonState::Idle;
                }
                break;
        }
    }
}

void IRAM_ATTR Button::isrHandler(void* arg) {
    auto* self = static_cast<Button*>(arg);
    self->interruptFlag_ = true;
}
