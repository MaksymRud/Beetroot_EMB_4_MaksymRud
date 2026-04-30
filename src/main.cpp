#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "Button.h"

#define Button_Pin 4
#define LogicAnalizer_Pin 21

extern "C" void app_main(void) {
    // Configure logic analyser output pin
    ButtonFSMESPIDF button(Button_Pin, 50, 50, 50, true);
    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    const uart_port_t uart_num = UART_NUM_1;
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19, DTR: UNUSED, DSR: UNUSED)
    ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_NO_CHANGE,
                                             UART_PIN_NO_CHANGE,
                                             UART_PIN_NO_CHANGE,
                                             UART_PIN_NO_CHANGE,
                                             UART_PIN_NO_CHANGE,
                                             UART_PIN_NO_CHANGE));

    uint8_t *data = (uint8_t *) malloc(uart_buffer_size / 2);

    while (1) {
        button.update();
        if (button.press_registered) {
            // Write data to UART.
            char* test_str = "Fire LED!\n";
            uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));
        }
        int len = uart_read_bytes(uart_num, data, uart_buffer_size / 2 - 1, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = '\0'; // Null-terminate the string
            led.toggle();
            printf("Received: %s\n", (char*)data);
        }
    }
}