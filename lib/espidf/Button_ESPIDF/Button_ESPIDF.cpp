#include "Button_ESPIDF.h"

// ── ButtonSimpleESPIDF ────────────────────────────────────────────

bool ButtonSimpleESPIDF::isrServiceInstalled_ = false;

ButtonSimpleESPIDF::ButtonSimpleESPIDF(uint8_t pin, uint16_t debounce_time,
                                       bool pullup)
    : ButtonSimple(pin, debounce_time), acceptCount_(0), pullup_(pullup) {}

void ButtonSimpleESPIDF::init() {
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = (1ULL << pin_);
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = pullup_ ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = pullup_ ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE;
    cfg.intr_type = pullup_ ? GPIO_INTR_NEGEDGE : GPIO_INTR_POSEDGE;
    gpio_config(&cfg);

    if (!isrServiceInstalled_) {
        gpio_install_isr_service(0);
        isrServiceInstalled_ = true;
    }
    gpio_isr_handler_add((gpio_num_t)pin_, isrHandler, this);
}

void ButtonSimpleESPIDF::update() {
    if (interruptFlag_) {
        if ((uint32_t)(esp_timer_get_time() - lastInterruptTime_) >= debounce_time_) {
            acceptCount_++;
            printf("Button pressed! Accepted: %d, Time since last: %lu us\n",
                            acceptCount_, (uint32_t)(esp_timer_get_time() - lastInterruptTime_));
            interruptFlag_ = false;
        }
    }
}

bool ButtonSimpleESPIDF::readButtonState() {
    return pullup_ ? !readPin() : readPin();
}

bool ButtonSimpleESPIDF::readPin() { return gpio_get_level((gpio_num_t)pin_) == 1; }

void IRAM_ATTR ButtonSimpleESPIDF::isrHandler(void* arg) {
    auto* self = static_cast<ButtonSimpleESPIDF*>(arg);
    self->lastInterruptTime_ = (uint32_t)(esp_timer_get_time());
    self->interruptFlag_ = true;
}

// ── ButtonFSMESPIDF ───────────────────────────────────────────────

bool ButtonFSMESPIDF::isrServiceInstalled_ = false;

ButtonFSMESPIDF::ButtonFSMESPIDF(uint8_t pin, uint16_t debounce_time,
                                   uint16_t short_press_time,
                                   uint16_t long_press_time, bool pullup)
    : ButtonFSM(pin, debounce_time, short_press_time, long_press_time),
      pullup_(pullup) {}

void ButtonFSMESPIDF::init() {
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = (1ULL << pin_);
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = pullup_ ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = pullup_ ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE;
    cfg.intr_type = pullup_ ? GPIO_INTR_NEGEDGE : GPIO_INTR_POSEDGE;
    gpio_config(&cfg);

    if (!isrServiceInstalled_) {
        gpio_install_isr_service(0);
        isrServiceInstalled_ = true;
    }
    gpio_isr_handler_add((gpio_num_t)pin_, isrHandler, this);
}

void ButtonFSMESPIDF::update() {
    uint32_t currentTime = (uint32_t)(esp_timer_get_time() / 1000);
    bool pressed = readButtonState();

    switch (state_) {
        case ButtonState::Idle:
            if (pressed) {
                state_ = ButtonState::Pressed;
                lastChangeTime_ = currentTime;
            }
            break;

        case ButtonState::Pressed:
            if (!pressed) {
                state_ = ButtonState::Released;
                lastChangeTime_ = currentTime;
            } else if (currentTime - lastChangeTime_ >= long_press_time_) {
                state_ = ButtonState::Held;
            }
            break;

        case ButtonState::Held:
            if (!pressed) {
                state_ = ButtonState::Released;
                lastChangeTime_ = currentTime;
            }
            break;

        case ButtonState::Released:
            if (pressed) {
                state_ = ButtonState::Pressed;
                lastChangeTime_ = currentTime;
            } else if (currentTime - lastChangeTime_ >= debounce_time_) {
                state_ = ButtonState::Idle;
            }
            break;
    }
}

bool ButtonFSMESPIDF::readButtonState() {
    return pullup_ ? !readPin() : readPin();
}

bool ButtonFSMESPIDF::readPin() { return gpio_get_level((gpio_num_t)pin_) == 1; }

void IRAM_ATTR ButtonFSMESPIDF::isrHandler(void* arg) {
    auto* self = static_cast<ButtonFSMESPIDF*>(arg);
    self->lastInterruptTime_ = (uint32_t)(esp_timer_get_time());
    self->interruptFlag_ = true;
}
