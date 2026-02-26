#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_STRIP_PIN 21
#define NUMPIXELS     1
#define ADC_INPUT_PIN 4  // YD-ESP32-S3 onboard WS2812 (IO48)

bool roomIsDark = false;
unsigned long before = millis();
unsigned long now = millis();

Adafruit_NeoPixel pixels(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

uint32_t Wheel(byte pos) {
    pos = 255 - pos;
    if(pos < 85) {
        return pixels.Color(255 - pos * 3, 0, pos * 3);
    } else if(pos < 170) {
        pos -= 85;
        return pixels.Color(0, pos * 3, 255 - pos * 3);
    } else {
        pos -= 170;
        return pixels.Color(pos * 3, 255 - pos * 3, 0);
    }
}

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
    pinMode(ADC_INPUT_PIN, INPUT);
    pixels.begin();
    pixels.setBrightness(100); // Brightness (0~255)
    pixels.show(); // Initialize all pixels to 'off'
    Serial.println("Rainbow demo start with brightness 100");
}


void loop() {
    float actual_voltage = 0.0;
    unsigned long now = millis();

    if ((now - before) >= 10000) {
        before = now;
        actual_voltage = analogReadMilliVolts(ADC_INPUT_PIN) / 1000.0;
        Serial.printf("Voltage: %.3f V\n", actual_voltage);
        if (actual_voltage < 1.0) {
            roomIsDark = true;
        } else {
            roomIsDark = false;
        }
    }

    if (roomIsDark) {
        pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Red
    } else {
        pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Green
    }
    pixels.show();
}
