#include <Arduino.h>
#include <atomic>

constexpr uint8_t Button_Pin = 6;
volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime = 0;
std::atomic<int> interrupts_counter(0);
hw_timer_t *Timer = nullptr;
constexpr unsigned long WorkingTime_us = 30000000;
volatile bool timerExpired = false;

/*
    Task 1: no debounce
*/

void IRAM_ATTR handleButtonInterrupt() {
    interrupts_counter.fetch_add(1);
    lastButtonInterruptTime = millis();
    buttonPressed = true;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(Button_Pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Button_Pin), handleButtonInterrupt, FALLING);
}

void loop() {
    if (buttonPressed) {
        Serial.printf("Button pressed! Interrupts count: %d, Time difference: %lu ms\n", interrupts_counter.load(), millis() - lastButtonInterruptTime);
        buttonPressed = false;
    }
}
