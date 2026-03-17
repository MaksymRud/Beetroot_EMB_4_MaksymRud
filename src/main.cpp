#include <Arduino.h>
#include <atomic>

constexpr uint8_t Button_Pin = 6; // ms
volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime = 0;
std::atomic<int> control_input_time(0);
/*
    Task 1: no debounce
*/

void IRAM_ATTR handleButtonInterrupt() {
    control_input_time.fetch_add(1);
    lastButtonInterruptTime = millis();
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(Button_Pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Button_Pin), handleButtonInterrupt, FALLING);
}

void loop() {

}
