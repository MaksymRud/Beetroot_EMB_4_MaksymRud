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
constexpr unsigned long debounceDelay_us = 50000;

volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime = 0;
unsigned long buttonStartWaitAcceptanceTime_us = 0;
unsigned long lastButtonStateChangeTime = 0;
std::atomic<int> interrupts_counter(0);
std::atomic<int> accepted_counter(0);
volatile bool timerExpired = false;
uint8_t buttonState = ButtonIdle;

/*
    Task 3: Button debounce using a finite state machine (FSM)

    An ISR attached to the FALLING edge sets a flag and records the
    interrupt timestamp.  The main loop then drives a four-state FSM
    that filters contact bounce by requiring the pin to remain stable
    for a full debounce interval (50 ms) before accepting a transition:

      Idle  ──(pin LOW)──▸  Pressed  ──(stable LOW ≥ 50 ms)──▸  Held
      Held  ──(pin HIGH)──▸ Released ──(stable HIGH ≥ 50 ms)──▸ Idle

    On the Released → Idle transition the press is counted and the
    elapsed time from the first interrupt to acceptance is printed.
    A logic-analyser output pin mirrors the button state in real time.
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
                } else {
                    lastButtonStateChangeTime = micros();
                    buttonState = ButtonState::ButtonReleased;
                }
                break;
            case ButtonState::ButtonReleased:
                if (pinState == HIGH) {
                    if (micros() - lastButtonStateChangeTime >= debounceDelay_us) {
                        buttonState = ButtonState::ButtonIdle;
                        accepted_counter.fetch_add(1);
                        unsigned long dt = micros() - buttonStartWaitAcceptanceTime_us;
                        Serial.printf("Button pressed (pin LOW)! Accepted: %d, Raw interrupts: %d, ISR->accept: %lu ms\n",
                          accepted_counter.load(), interrupts_counter.load(), dt / 1000);
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
