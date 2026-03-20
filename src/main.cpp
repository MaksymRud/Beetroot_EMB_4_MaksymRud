#include <Arduino.h>
#include "Led.h"

constexpr uint8_t GREEN_LED_PIN = 5;
constexpr uint8_t YELLOW_LED_PIN = 4;
constexpr uint8_t RED_LED_PIN = 10;

Led greenLed;
Led yellowLed;
Led redLed;

void setup() {
    Serial.begin(115200);
    greenLed = Led(GREEN_LED_PIN, 200, 100);
    yellowLed = Led(YELLOW_LED_PIN, 500, 100);
    redLed = Led(RED_LED_PIN, 1000, 100);
}

void loop() {
    greenLed.update();
    yellowLed.update();
    redLed.update();
}
