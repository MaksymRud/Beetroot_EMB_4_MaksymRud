#include <Arduino.h>
#include <atomic>
#include "Button_FSM.h"

constexpr uint8_t Button_Pin = 6;
constexpr unsigned long POLL_INTERVAL_MS = 5;

volatile unsigned long lastButtonInterruptTime = 0;
std::atomic<int> interrupts_counter(0);
int accepted_counter = 0;

hw_timer_t *Timer = nullptr;
constexpr long WorkingTime = 30000000;
volatile bool timerExpired = false;

Button_FSM_t button;
unsigned long lastPollTime = 0;

/*
    Task 4: polling + FSM debounce
    - ISR only counts raw interrupts and records timestamp
    - loop() polls pin every 5 ms via Button_FSM_Update
*/

void ARDUINO_ISR_ATTR onStopFanTimer() {
    timerExpired = true;
}

void IRAM_ATTR handleButtonInterrupt() {
    interrupts_counter.fetch_add(1);
    lastButtonInterruptTime = millis();
}

void startTimer() {
    Timer = timerBegin(0, 80, true);
    timerAttachInterrupt(Timer, &onStopFanTimer, true);
    timerAlarmWrite(Timer, WorkingTime, true);
    timerAlarmEnable(Timer);
    Serial.println("Timer started...");
}

void setup() {
    Serial.begin(115200);
    Button_FSM_Init(&button, Button_Pin, 30, 50, 800);
    attachInterrupt(digitalPinToInterrupt(Button_Pin), handleButtonInterrupt, FALLING);
    startTimer();
}

void loop() {
    unsigned long currentPollingTime = millis();

    if (currentPollingTime - lastPollTime >= POLL_INTERVAL_MS) {
        lastPollTime = currentPollingTime;
        Button_FSM_Update(&button);

        bool isLong = false;
        if (Button_FSM_IsPressed(&button, isLong)) {
            accepted_counter++;
            unsigned long dt = millis() - lastButtonInterruptTime;
            Serial.printf("Button %s press! Accepted: %d, Raw interrupts: %d, ISR->accept: %lu ms\n",
                          isLong ? "LONG" : "SHORT",
                          accepted_counter, interrupts_counter.load(), dt);
        }
    }
    if (timerExpired) {
        Serial.printf("Timer expired! Accepted: %d, Raw interrupts: %d\n",
                      accepted_counter, interrupts_counter.load());
        timerExpired = false;
    }
}
