#include <Arduino.h>
#include <atomic>

#define Button_Pin 6
#define LogicAnalizer_Pin 21

volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime_us = 0;
std::atomic<int> interrupts_counter(0);
int accept_button_counter = 0;

/*
    Task 1: no debounce
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
    pinMode(LogicAnalizer_Pin, OUTPUT);
    digitalWrite(LogicAnalizer_Pin, LOW);
    attachInterrupt(digitalPinToInterrupt(Button_Pin), handleButtonInterrupt, FALLING);
}

void loop() {
    if (buttonPressed) {
        accept_button_counter++;
        Serial.printf("Button pressed! Accepted count: %d, Interrupts count: %d, Time difference: %lu us\n", accept_button_counter, interrupts_counter.load(), (micros() - lastButtonInterruptTime_us));
        buttonPressed = false;
    }
    digitalWrite(LogicAnalizer_Pin, digitalRead(Button_Pin));
}
