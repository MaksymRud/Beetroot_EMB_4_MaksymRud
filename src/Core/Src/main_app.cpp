#include "main.h"
#include <stdint.h>

volatile bool fire_led = false;

enum class ButtonState : uint8_t {
    Idle,
    Pressed,
    Held,
    Released
};

class ButtonSimpleSTM32 {
private:
    void transmit()
    {
        HAL_UART_Transmit(huart_, (uint8_t*)"OK\r\n", 4, 100);
    }

    UART_HandleTypeDef* huart_;
    ButtonState state_ {ButtonState::Idle};
    GPIO_TypeDef* port_;
    const uint16_t pin_;
    uint16_t debounceTime_;
    bool interruptFlag {false};
    uint32_t lastButtonStateChangeTimeMs_;

public:
    explicit ButtonSimpleSTM32(UART_HandleTypeDef* huart, GPIO_TypeDef* port,  uint16_t pin, uint16_t debounceTime): 
    huart_(huart), port_(port), pin_(pin), debounceTime_(debounceTime) { }

    void update()
    {
        if (interruptFlag) {
            int pinState = HAL_GPIO_ReadPin(port_, pin_);
            switch (state_)
            {
                case ButtonState::Idle:
                    if (pinState == 0) {
                        lastButtonStateChangeTimeMs_ = HAL_GetTick();
                        state_ = ButtonState::Pressed;
                    } else {
                        state_ = ButtonState::Idle;
                    }
                    break;
                case ButtonState::Pressed:
                    if (pinState == 0) {
                        if (HAL_GetTick() - lastButtonStateChangeTimeMs_ >= debounceTime_) {
                            state_ = ButtonState::Held;
                        }
                    }
                    break;
                case ButtonState::Held:
                    if (pinState == 0) {
                        state_ = ButtonState::Held;
                    } else {
                        lastButtonStateChangeTimeMs_ = HAL_GetTick();
                        state_ = ButtonState::Released;
                    }
                    break;
                case ButtonState::Released:
                    if (pinState == 0) {
                        if (HAL_GetTick() - lastButtonStateChangeTimeMs_ >= debounceTime_) {
                            state_ = ButtonState::Idle;
                            interruptFlag = false;
                        }
                    };
                    break;
                default:
                    break;
            }
        }
    }

    static void isrHandler(void* arg)
    {
        auto* self = static_cast<ButtonSimpleSTM32*>(arg);
        self->interruptFlag = true;
    }
};

class Led {
private:
    GPIO_TypeDef* port_;
    const uint16_t pin_;

public:
    Led(GPIO_TypeDef* port, uint16_t pin): port_(port), pin_(pin) { }
    void toggle() { HAL_GPIO_TogglePin(port_, pin_); }
    void on() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET); }
    void off() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET); }
};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // Цей callback викликається, коли UART отримав дані
    if (huart->Instance == USART2) {
        fire_led = true;
    }
}


extern "C"
{
    void main_cpp()
    {
        Led led(LedPin_GPIO_Port, LedPin_Pin);
        ButtonSimpleSTM32 button(&huart2, ButtonPin_GPIO_Port, ButtonPin_Pin, 50);

        while(1)
        {
            button.update();

            if (fire_led)
            {
                led.toggle();
                fire_led = false;
            }
        }
    }
}
