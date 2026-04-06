#include "Button_Arduino.h"

// ── Arduino ISR handlers ──────────────────────────────────────────

void IRAM_ATTR ButtonSimpleArduino::isrHandler(void* arg) {
    auto* self = static_cast<ButtonSimpleArduino*>(arg);
    self->interruptCount_++;
    self->lastInterruptTime_ = micros();
    self->interruptFlag_ = true;
}

void IRAM_ATTR ButtonFSMArduino::isrHandler(void* arg) {
    auto* self = static_cast<ButtonFSMArduino*>(arg);
    self->interruptCount_++;
    self->lastInterruptTime_ = micros();
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
    uint32_t currentTime = millis();
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
