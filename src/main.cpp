#include <Arduino.h>

constexpr uint8_t Relay_Control_Pin = 4;
constexpr uint8_t Reley_Open_Pin = 38;
constexpr uint8_t Button_Pin = 6; // ms
volatile bool relayIsOpen = false;
volatile bool buttonPressed = false;
unsigned long control_input_time = 0;
volatile unsigned long lastButtonInterruptTime = 0;
volatile unsigned long lastRelayInterruptTime = 0;
unsigned long relay_open_time = 0;

void IRAM_ATTR handleRelayOpenInterrupt() {
    unsigned long currentTime = millis();
    if (currentTime - lastRelayInterruptTime > 50) {
        lastRelayInterruptTime = currentTime;
        relayIsOpen = true;
    }
}

void IRAM_ATTR handleButtonInterrupt() {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonInterruptTime > 50) {
        lastButtonInterruptTime = currentTime;
        buttonPressed = true;
    }
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(Relay_Control_Pin, OUTPUT);
    pinMode(Reley_Open_Pin, INPUT_PULLDOWN);
    pinMode(Button_Pin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(Button_Pin), handleButtonInterrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(Reley_Open_Pin), handleRelayOpenInterrupt, RISING);
    digitalWrite(Relay_Control_Pin, LOW);
}

void loop() {
    if (buttonPressed) {
        if (digitalRead(Relay_Control_Pin) == LOW) {
            digitalWrite(Relay_Control_Pin, HIGH);
            control_input_time = millis();
            Serial.printf("Try to open relay! Time: %lu ms\n", control_input_time);
        } else {
            digitalWrite(Relay_Control_Pin, LOW);
            Serial.printf("Try to close relay!\n");
        }
        buttonPressed = false;
    }

    if (relayIsOpen) {
        relayIsOpen = false;
        relay_open_time = millis();
        if (digitalRead(Relay_Control_Pin) == LOW) {
            Serial.printf("Relay debounced on closing!\n");
        } else {
            Serial.printf("Relay opened! Time since control input: %lu ms\n", relay_open_time - control_input_time);
        }
    }
}
