#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_STRIP_PIN 21
#define NUMPIXELS     5
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

bool IsRoomDark() {
    float actual_voltage = analogReadMilliVolts(ADC_INPUT_PIN) / 1000.0;
    Serial.printf("Voltage: %.3f V\n", actual_voltage);
    return actual_voltage < 1.0 ? true : false;
}

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
    pinMode(ADC_INPUT_PIN, INPUT);
    pixels.begin();
    pixels.setBrightness(50);
    pixels.show();
    Serial.println("Rainbow demo start with brightness 50");
}
\


void loop() {
    unsigned long now = millis();

    if ((now - before) >= 3000) {
        before = now;
        roomIsDark = IsRoomDark();
    }

    if (roomIsDark) {
        for(int i = 0; i < 256; i++) {
            for (int j = 0; j < NUMPIXELS; j++) {
                pixels.setPixelColor(j,Wheel(i));
                pixels.show();
                delay(5);
                if (!IsRoomDark()) {
                    roomIsDark = false;
                    break;
                }
            }
        }
    } else {
        for (int j = 0; j < NUMPIXELS; j++) {
            pixels.setPixelColor(j, pixels.Color(0, 0, 0));
            pixels.show();
            delay(5);
            if (IsRoomDark()) {
                roomIsDark = true;
                break;
            }
        }
    }
    
}
