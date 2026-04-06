#include "Button_ESPIDF.h"

// ── Static flags ──────────────────────────────────────────────────
bool ButtonSimpleESPIDF::isrServiceInstalled_ = false;
bool ButtonFSMESPIDF::isrServiceInstalled_    = false;

// ── init() implementations ────────────────────────────────────────

void ButtonSimpleESPIDF::init() {
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = (1ULL << pin_);
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = pullup_ ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = pullup_ ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE;
    cfg.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&cfg);

    if (!isrServiceInstalled_) {
        gpio_install_isr_service(0);
        isrServiceInstalled_ = true;
    }
    gpio_isr_handler_add((gpio_num_t)pin_, isrHandler, this);
}

void ButtonFSMESPIDF::init() {
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = (1ULL << pin_);
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = pullup_ ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = pullup_ ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE;
    cfg.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&cfg);

    if (!isrServiceInstalled_) {
        gpio_install_isr_service(0);
        isrServiceInstalled_ = true;
    }
    gpio_isr_handler_add((gpio_num_t)pin_, isrHandler, this);
}

// ── ISR handlers ──────────────────────────────────────────────────

void IRAM_ATTR ButtonSimpleESPIDF::isrHandler(void* arg) {
    auto* self = static_cast<ButtonSimpleESPIDF*>(arg);
    self->interruptCount_++;
    self->lastInterruptTime_ = (uint32_t)(esp_timer_get_time());
    self->interruptFlag_ = true;
}

void IRAM_ATTR ButtonFSMESPIDF::isrHandler(void* arg) {
    auto* self = static_cast<ButtonFSMESPIDF*>(arg);
    self->interruptCount_++;
    self->lastInterruptTime_ = (uint32_t)(esp_timer_get_time());
    self->interruptFlag_ = true;
}

// ── Shared logic (defined once per translation unit) ───────────────

void ButtonSimple::update() {
    pressed_ = readButtonState();
}

bool ButtonSimple::isPressed() {
    bool p = pressed_;
    pressed_ = false;
    return p;
}

void ButtonFSM::update() {
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

bool ButtonFSM::isPressed(bool &is_long_press) {
    if (state_ == ButtonState::Idle && lastChangeTime_ != 0) {
        is_long_press = false;
        lastChangeTime_ = 0;
        return true;
    }
    if (state_ == ButtonState::Held) {
        is_long_press = true;
        return true;
    }
    is_long_press = false;
    return false;
}
