#include <Arduino.h>
#include "TrafficLight.h"

constexpr uint8_t GREEN_LED_PIN = 5;
constexpr uint8_t YELLOW_LED_PIN = 4;
constexpr uint8_t RED_LED_PIN = 10;

TrafficLight trafficLight;

void setup() {
    Serial.begin(115200);
    trafficLight = TrafficLight(RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN);
}

void loop() {
    trafficLight.update();
}
