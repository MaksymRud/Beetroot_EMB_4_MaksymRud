#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

enum class ButtonState : uint8_t {
    Idle,
    Pressed,
    Held,
    Released
};

class ButtonBase {
protected:
    uint8_t pin_;
    uint16_t debounce_time_;
    uint32_t lastChangeTime_;

    virtual bool readPin() = 0;

public:
    ButtonBase(uint8_t pin, uint16_t debounce_time)
        : pin_(pin), debounce_time_(debounce_time), lastChangeTime_(0),
          interruptFlag_(false), lastInterruptTime_(0) {}
    virtual ~ButtonBase() = default;

    virtual void update() = 0;
    virtual void init() = 0;
    virtual bool readButtonState() = 0;

    volatile bool interruptFlag_;
    volatile uint32_t lastInterruptTime_;
};

class ButtonSimple : public ButtonBase {
public:
    ButtonSimple(uint8_t pin, uint16_t debounce_time)
        : ButtonBase(pin, debounce_time) {}
};

// Full FSM button with short/long press detection
class ButtonFSM : public ButtonBase {
public:
    ButtonFSM(uint8_t pin, uint16_t debounce_time,
              uint16_t short_press_time, uint16_t long_press_time)
        : ButtonBase(pin, debounce_time),
          short_press_time_(short_press_time),
          long_press_time_(long_press_time),
          state_(ButtonState::Idle) {}
    ButtonState state() const { return state_; }

protected:
    uint16_t short_press_time_;
    uint16_t long_press_time_;
    ButtonState state_;
};

#endif // BUTTON_H
