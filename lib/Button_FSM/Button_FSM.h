#ifndef BUTTON_FSM_H
#define BUTTON_FSM_H

#include <Arduino.h>

typedef enum {
    STATE_IDLE,
    STATE_PRESSED,
    STATE_HELD,
    STATE_RELEASED
} ButtonState_t;

typedef struct {
    uint8_t pin;
    ButtonState_t state;
    uint16_t debounce_time;
    uint16_t short_press_time;
    uint16_t long_press_time;
    uint32_t lastChangeTime;
    bool pressDetected;
    bool longPressDetected;
} Button_FSM_t;

void Button_FSM_Init(Button_FSM_t *button, uint8_t pin, uint16_t debounce_time, uint16_t short_press_time, uint16_t long_press_time);
void Button_FSM_Update(Button_FSM_t *button);
bool Button_FSM_IsPressed(Button_FSM_t *button, bool &is_long_press);

#endif // BUTTON_FSM_H