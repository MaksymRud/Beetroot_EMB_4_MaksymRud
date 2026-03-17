#include <Arduino.h>
#include <atomic>

constexpr uint8_t Button_Pin = 6;
volatile bool buttonPressed = false;
volatile unsigned long lastButtonInterruptTime = 0;
std::atomic<int> interrupts_counter(0);
std::atomic<int> accepted_counter(0);
hw_timer_t *Timer = nullptr;
constexpr long WorkingTime = 30000000;
volatile bool timerExpired = false;

/*
    Task 3: state-based debounce using pin level
    - ISR sets flag on FALLING edge
    - loop() reads actual pin level:
      LOW  (pressed)  → accept the press
      HIGH (released) → ignore (bounce / release)
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

        int pinState = digitalRead(Button_Pin);

        if (pinState == LOW) {                       // button is still held → real press
            accepted_counter.fetch_add(1);
            unsigned long dt = millis() - lastButtonInterruptTime;
            Serial.printf("Button pressed (pin LOW)! Accepted: %d, Raw interrupts: %d, ISR->accept: %lu ms\n",
                          accepted_counter.load(), interrupts_counter.load(), dt);
        } else {                                     // pin already HIGH → bounce / release
            Serial.printf("Bounce ignored (pin HIGH). Raw interrupts: %d\n",
                          interrupts_counter.load());
        }
    }
    if (timerExpired) {
        Serial.printf("Timer expired! Accepted: %d, Raw interrupts: %d\n",
                      accepted_counter.load(), interrupts_counter.load());
        timerExpired = false;
    }
}
