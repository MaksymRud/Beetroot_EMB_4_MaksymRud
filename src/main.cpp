#ifdef ARDUINO
// ── Arduino framework ──────────────────────────────────────────────
#include <Arduino.h>
#include "Button_Arduino.h"

#define Button_Pin 6
#define LogicAnalizer_Pin 21

static ButtonFSMArduino button(Button_Pin, 50, 50, 50, INPUT_PULLUP);

void setup() {
    Serial.begin(115200);
    pinMode(LogicAnalizer_Pin, OUTPUT);
    digitalWrite(LogicAnalizer_Pin, LOW);
    button.init();   // configures pin + attaches ISR internally
}

void loop() {
    button.update();
    digitalWrite(LogicAnalizer_Pin, digitalRead(Button_Pin));
}

#else
// ── ESP-IDF framework ─────────────────────────────────────────────
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "Button_ESPIDF.h"

#define Button_Pin 6
#define LogicAnalizer_Pin 21

extern "C" void app_main(void) {
    // Configure logic analyser output pin
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << LogicAnalizer_Pin);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level((gpio_num_t)LogicAnalizer_Pin, 0);

    ButtonFSMESPIDF button(Button_Pin, 50, 50, 50, true);
    button.init();

    while (1) {
        button.update();
        gpio_set_level((gpio_num_t)LogicAnalizer_Pin,
                       gpio_get_level((gpio_num_t)Button_Pin));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#endif
