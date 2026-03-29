#include <Arduino.h>
#include <atomic>

enum ButtonState {
    ButtonPressed,
    ButtonHeld,
    ButtonReleased,
    ButtonIdle
};

constexpr uint8_t Button_Pin = 6;
constexpr uint8_t LogicAnalyzer_Pin = 21;
constexpr unsigned long debounceDelay_us = 50000; // debounce delay in microseconds
volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime = 0;
unsigned long buttonAcceptedTime_us = 0;
unsigned long buttonStartWaitAcceptanceTime_us = 0;
unsigned long lastButtonStateChangeTime = 0;
std::atomic<int> interrupts_counter(0);
std::atomic<int> accepted_counter(0);
volatile bool timerExpired = false;
uint8_t buttonState = ButtonIdle;

/*
    Task 3: state-based debounce using pin level
    - ISR sets flag on FALLING edge
    - loop() reads actual pin level:
      LOW  (pressed)  → accept the press
      HIGH (released) → ignore (bounce / release)
*/

void IRAM_ATTR handleButtonInterrupt() {
    interrupts_counter.fetch_add(1);
    lastButtonInterruptTime = micros();
    buttonPressed = true;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(Button_Pin, INPUT_PULLUP);
    pinMode(LogicAnalyzer_Pin, OUTPUT);
    digitalWrite(LogicAnalyzer_Pin, LOW);
    attachInterrupt(digitalPinToInterrupt(Button_Pin), handleButtonInterrupt, FALLING);
}

void loop() {
    if (buttonPressed) {
        int pinState = digitalRead(Button_Pin);
        switch (buttonState)
        {
            case ButtonState::ButtonIdle:
                if (pinState == LOW) {
                    lastButtonStateChangeTime = micros();
                    buttonStartWaitAcceptanceTime_us = micros();
                    buttonState = ButtonState::ButtonPressed;
                } else {
                    buttonState = ButtonState::ButtonIdle;
                }
                break;
            case ButtonState::ButtonPressed:
                if (pinState == LOW) {
                    if (micros() - lastButtonStateChangeTime >= debounceDelay_us) {
                        buttonState = ButtonState::ButtonHeld;
                    }
                }
                break;
            case ButtonState::ButtonHeld:
                if (pinState == LOW) {
                    buttonState = ButtonState::ButtonHeld;
                    accepted_counter.fetch_add(1);
                    unsigned long dt = micros() - buttonStartWaitAcceptanceTime_us;
                    Serial.printf("Button pressed (pin LOW)! Accepted: %d, Raw interrupts: %d, ISR->accept: %lu ms\n",
                          accepted_counter.load(), interrupts_counter.load(), dt);
                } else {
                    lastButtonStateChangeTime = micros();
                    buttonState = ButtonState::ButtonReleased;
                }
                break;
            case ButtonState::ButtonReleased:
                if (pinState == HIGH) {
                    if (micros() - lastButtonStateChangeTime >= debounceDelay_us) {
                        buttonState = ButtonState::ButtonIdle;
                        buttonPressed = false;
                    }
                };
                break;
            default:
                break;
        }
    }
    digitalWrite(LogicAnalyzer_Pin, digitalRead(Button_Pin));
}
