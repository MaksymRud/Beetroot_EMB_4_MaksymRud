#include "Button_Arduino.h"

// ── ButtonSimpleArduino ───────────────────────────────────────────

ButtonSimpleArduino::ButtonSimpleArduino(uint8_t pin, uint16_t debounce_time_ms,
                                         uint8_t mode)
    : ButtonSimple(pin, debounce_time_ms),
      mode(mode), acceptCount_(0) {}

void ButtonSimpleArduino::init() {
    pinMode(pin_, mode);
    attachInterruptArg(digitalPinToInterrupt(pin_), isrHandler, this, FALLING);
}

void ButtonSimpleArduino::update() {
    if (interruptFlag_) {
        if (micros() - lastInterruptTime_ >= debounce_time_) {
            acceptCount_++;
            Serial.printf("Button pressed! Accepted: %d, Time since last: %lu us\n",
                            acceptCount_, micros() - lastInterruptTime_);
            interruptFlag_ = false;
        }
    }
}

bool ButtonSimpleArduino::readButtonState() {
    return (mode == INPUT_PULLUP) ? !readPin() : readPin();
}

bool ButtonSimpleArduino::readPin() { return digitalRead(pin_); }

void IRAM_ATTR ButtonSimpleArduino::isrHandler(void* arg) {
    auto* self = static_cast<ButtonSimpleArduino*>(arg);
    self->lastInterruptTime_ = micros();
    self->interruptFlag_ = true;
}

// ── ButtonFSMArduino ──────────────────────────────────────────────

ButtonFSMArduino::ButtonFSMArduino(uint8_t pin, uint16_t debounce_time,
                                     uint16_t short_press_time,
                                     uint16_t long_press_time, uint8_t mode)
    : ButtonFSM(pin, debounce_time, short_press_time, long_press_time),
      mode(mode) {}

void ButtonFSMArduino::init() {
    pinMode(pin_, mode);
    attachInterruptArg(digitalPinToInterrupt(pin_), isrHandler, this, FALLING);
}

void ButtonFSMArduino::update() {
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

bool ButtonFSMArduino::readButtonState() {
    return (mode == INPUT_PULLUP) ? !readPin() : readPin();
}

bool ButtonFSMArduino::readPin() { return digitalRead(pin_); }

void IRAM_ATTR ButtonFSMArduino::isrHandler(void* arg) {
    auto* self = static_cast<ButtonFSMArduino*>(arg);
    self->lastInterruptTime_ = micros();
    self->interruptFlag_ = true;
}
