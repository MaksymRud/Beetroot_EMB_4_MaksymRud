#include <Arduino.h>
#include <atomic>

constexpr uint8_t Button_Pin = 6; // ms
volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime = 0;
std::atomic<int> interrupts_counter(0);
hw_timer_t *Timer = nullptr;
constexpr long WorkingTime = 30000000;
volatile bool timerExpired = false;

/*
    Task 1: no debounce
*/

void ARDUINO_ISR_ATTR onStopFanTimer() {
    timerExpired = true;
}

void IRAM_ATTR handleButtonInterrupt() {
    interrupts_counter.fetch_add(1);
    lastButtonInterruptTime = millis();
    buttonPressed = true;
}

void startTimer() {
    Timer = timerBegin(0, 80, true);
    timerAttachInterrupt(Timer, &onStopFanTimer, true);
    timerAlarmWrite(Timer, WorkingTime, true);
    timerAlarmEnable(Timer);
    Serial.println("Timer started...");
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(Button_Pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Button_Pin), handleButtonInterrupt, FALLING);
    startTimer();
}

void loop() {
    if (buttonPressed) {
        Serial.printf("Button pressed! Interrupts count: %d, Time difference: %lu ms\n", interrupts_counter.load(), millis() - lastButtonInterruptTime);
        buttonPressed = false;
    }
    if (timerExpired) {
        Serial.printf("Timer expired! All interrupts: %d\n", interrupts_counter.load());
        timerExpired = false;
    }
}
