#include <Arduino.h>
#include <atomic>

constexpr uint8_t Button_Pin = 6;
constexpr unsigned long DebounceDelay = 50; // ms
volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime = 0;
unsigned long lastAcceptedPressTime = 0;
std::atomic<int> interrupts_counter(0);
std::atomic<int> accepted_counter(0);
hw_timer_t *Timer = nullptr;
constexpr long WorkingTime = 30000000;
volatile bool timerExpired = false;

/*
    Task 2: time-based software debounce
    - ISR only records timestamp and sets flag
    - Debounce check (< 50 ms) is done in loop()
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
        buttonPressed = false;
        unsigned long now = millis();
        if (now - lastAcceptedPressTime >= DebounceDelay) {
            lastAcceptedPressTime = now;
            accepted_counter.fetch_add(1);
            Serial.printf("Button pressed! Accepted: %d, Raw interrupts: %d, Time since last: %lu ms\n",
                          accepted_counter.load(), interrupts_counter.load(), now - lastButtonInterruptTime);
        } else {
            Serial.printf("Bounce ignored (dt=%lu ms). Raw interrupts: %d\n",
                          now - lastAcceptedPressTime, interrupts_counter.load());
        }
    }
    if (timerExpired) {
        Serial.printf("Timer expired! Accepted: %d, Raw interrupts: %d\n",
                      accepted_counter.load(), interrupts_counter.load());
        timerExpired = false;
    }
}
