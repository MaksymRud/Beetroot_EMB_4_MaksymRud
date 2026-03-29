#include <Arduino.h>
#include <atomic>

constexpr uint8_t Button_Pin = 6;
constexpr uint8_t LogicAnalyzer_Pin = 21;
constexpr unsigned long DebounceDelay_us = 50000;
volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime = 0;
unsigned long lastAcceptedPressTime = 0;
std::atomic<int> interrupts_counter(0);
std::atomic<int> accepted_counter(0);
volatile bool timerExpired = false;

/*
    Task 2: time-based software debounce
    - ISR only records timestamp and sets flag
    - Debounce check (< 50 ms) is done in loop()
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
        buttonPressed = false;
        unsigned long now = micros();
        if (now - lastAcceptedPressTime >= DebounceDelay_us) {
            lastAcceptedPressTime = now;
            accepted_counter.fetch_add(1);
            Serial.printf("Button pressed! Accepted: %d, Raw interrupts: %d, Time since last: %lu us\n",
                          accepted_counter.load(), interrupts_counter.load(), now - lastButtonInterruptTime);
        } else {
            Serial.printf("Bounce ignored (dt=%lu us). Raw interrupts: %d\n",
                          now - lastAcceptedPressTime, interrupts_counter.load());
        }
    }
    digitalWrite(LogicAnalyzer_Pin, !digitalRead(LogicAnalyzer_Pin)); // Toggle logic analyzer pin for timing visualization
}
