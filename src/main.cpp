#include <Arduino.h>

#define RED_LED 13  // YD-ESP32-S3 onboard WS2812 (IO48)
#define BLUE_LED 4   // One RGB LED only
#define BUTTON_MODE_UP 5
#define BUTTON_MODE_DOWN 0

int delay_modes_array[] = {100, 200, 500, 1000};
int modes_length = sizeof(delay_modes_array) / sizeof(delay_modes_array[0]);
int current_mode = 0;

void setup() {
    Serial.begin(115200);
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTON_MODE_UP, INPUT_PULLUP);
    pinMode(BUTTON_MODE_DOWN, INPUT_PULLUP);
}


void loop() {
    while (digitalRead(BUTTON_MODE_UP) != LOW && digitalRead(BUTTON_MODE_DOWN) != LOW) {
        // Wait for button release to avoid multiple triggers
        digitalWrite(RED_LED, HIGH);
        digitalWrite(BLUE_LED, LOW);
        delay(delay_modes_array[current_mode]);
        digitalWrite(RED_LED, LOW);
        digitalWrite(BLUE_LED, HIGH);
        delay(delay_modes_array[current_mode]);
        delay(10);
    }

    if (digitalRead(BUTTON_MODE_UP) == LOW) {
        Serial.println("Button UP pressed");
        delay(50);

        while (digitalRead(BUTTON_MODE_UP) == LOW) {
            delay(10);
        }
        current_mode += 1; // Cycle through modes
        if (current_mode >= modes_length) {
            current_mode = modes_length - 1; // Wrap around to the first mode
        }
        Serial.print("Current mode: ");
        Serial.println(current_mode);
    }

    if (digitalRead(BUTTON_MODE_DOWN) == LOW) {
        Serial.println("Button DOWN pressed");
        delay(50);

        while (digitalRead(BUTTON_MODE_DOWN) == LOW) {
            delay(10);
        }
        current_mode = (current_mode - 1);// Cycle through modes in reverse
        if (current_mode < 0) {
            current_mode = 0; // Wrap around to the last mode
        }
        Serial.print("Current mode: ");
        Serial.println(current_mode);
    }
}