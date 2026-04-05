#include <Arduino.h>
#include <atomic>

#define Button_Pin 6
#define LogicAnalyzer_Pin 21

constexpr unsigned long DebounceDelay_us = 50000;

volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime_us = 0;
std::atomic<int> interrupts_counter(0);
std::atomic<int> accepted_counter(0);

/*
    Task 2: time-based software debounce
    - ISR only records timestamp and sets flag
    - Debounce check (< 50 ms) is done in loop()
*/

void IRAM_ATTR handleButtonInterrupt() {
    interrupts_counter.fetch_add(1);
    lastButtonInterruptTime_us = micros();
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
        if (micros() - lastButtonInterruptTime_us >= DebounceDelay_us) {
            buttonPressed = false;
            accepted_counter.fetch_add(1);
            Serial.printf("Button pressed! Accepted: %d, Raw interrupts: %d, Time since last: %lu us\n",
                          accepted_counter.load(), interrupts_counter.load(), micros() - lastButtonInterruptTime_us);
        }
    }
    digitalWrite(LogicAnalyzer_Pin, digitalRead(Button_Pin));
}
