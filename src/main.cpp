#include <Arduino.h>

#define RED_LED 13  // YD-ESP32-S3 onboard WS2812 (IO48)
#define BLUE_LED 4   // One RGB LED only
#define BUTTON_MODE_UP 5

int16_t counter_left = 0;

void IRAM_ATTR reaction_left() {
  counter_left++;
  Serial.println("\nLevel is Rising! Count: " + String(counter_left));
}

void setup() {
  pinMode(BUTTON_MODE_UP, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(BUTTON_MODE_UP), reaction_left, RISING);
}

void loop() {
  Serial.println("Button is held down!");
  delay(250);
}
