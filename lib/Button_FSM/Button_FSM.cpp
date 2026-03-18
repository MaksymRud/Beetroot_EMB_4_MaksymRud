#include "Button_FSM.h"

void Button_FSM_Init(Button_FSM_t *button, uint8_t pin, uint16_t debounce_time, uint16_t short_press_time, uint16_t long_press_time) {
    button->pin = pin;
    button->state = STATE_IDLE;
    button->debounce_time = debounce_time;
    button->short_press_time = short_press_time;
    button->long_press_time = long_press_time;
    button->lastChangeTime = 0;
    button->pressDetected = false;
    button->longPressDetected = false;
    pinMode(pin, INPUT_PULLUP);
}

void Button_FSM_Update(Button_FSM_t *button) {
    uint32_t currentTime = millis();
    bool isPressed = digitalRead(button->pin) == LOW;

    switch (button->state) {
        case STATE_IDLE:
            if (isPressed) {
                button->state = STATE_PRESSED;
                button->lastChangeTime = currentTime;
            }
            break;

        case STATE_PRESSED:
            if (!isPressed) {
                if (currentTime - button->lastChangeTime >= button->short_press_time) {
                    button->pressDetected = true;
                }
                button->state = STATE_RELEASED;
                button->lastChangeTime = currentTime;
            } else if (currentTime - button->lastChangeTime >= button->long_press_time) {
                button->state = STATE_HELD;
                button->longPressDetected = true;
            }
            break;

        case STATE_HELD:
            if (!isPressed) {
                button->state = STATE_RELEASED;
                button->lastChangeTime = currentTime;
            }
            break;

        case STATE_RELEASED:
            if (isPressed) {
                button->state = STATE_PRESSED;
                button->lastChangeTime = currentTime;
            } else if (currentTime - button->lastChangeTime >= button->debounce_time) {
                button->state = STATE_IDLE;
            }
            break;
    }
}

bool Button_FSM_IsPressed(Button_FSM_t *button, bool &is_long_press) {
    if (button->longPressDetected) {
        button->longPressDetected = false;
        is_long_press = true;
        return true;
    }
    if (button->pressDetected) {
        button->pressDetected = false;
        is_long_press = false;
        return true;
    }
    return false;
}