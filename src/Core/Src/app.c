#include "app.h"
#include "main.h"

void setup() {
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void loop() {
    const char *msg = "LED blinked!\r\n";

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    CDC_Transmit_FS((uint8_t *)msg, strlen(msg));

    HAL_Delay(1000);
}