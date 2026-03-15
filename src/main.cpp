#include <Arduino.h>

constexpr uint8_t Relay_Control_Pin = 4;
constexpr uint8_t Reley_Open_Pin = 38;
constexpr uint8_t Button_Pin = 6; // ms
constexpr uint32_t Delay_Between_Experiments = 10000;
volatile bool relayIsOpen = false;
volatile bool buttonPressed = false;
unsigned long control_input_time = 0;
unsigned long relay_open_time = 0;

void IRAM_ATTR handleRelayOpenInterrupt() {
    relayIsOpen = true;
}

void IRAM_ATTR handleButtonInterrupt() {
    buttonPressed = true;
}

void setup() {
  // put your setup code here, to run once:
    pinMode(Relay_Control_Pin, OUTPUT);
    pinMode(Reley_Open_Pin, INPUT_PULLUP);
    pinMode(Button_Pin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(Button_Pin), handleButtonInterrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(Reley_Open_Pin), handleRelayOpenInterrupt, RISING);
    digitalWrite(Relay_Control_Pin, HIGH);
}

void loop() {
    if (buttonPressed) {
        buttonPressed = false;
        digitalWrite(Relay_Control_Pin, LOW);
        control_input_time = millis();
        Serial.printf("Try to open relay! Time: %lu ms\n", control_input_time);
    }

    if (relayIsOpen) {
        relayIsOpen = false;
        relay_open_time = millis();
        digitalWrite(Relay_Control_Pin, HIGH);
        Serial.printf("Relay opened! On time: %lu ms \nTime since control input: %lu ms\n", relay_open_time, relay_open_time - control_input_time);
    }
}
