#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "systick.h"
#include "stdbool.h"
#include "servo.h"

/**
 *
 */
void writeLED(bool_e b)
{
    HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

/**
 *
 * @return
 */
bool_e readButton(void)
{
    return !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}

static volatile uint32_t t = 0;
void process_ms(void)
{
    if(t)
        t--;
}

#define NUM_SERVOS 12

int main(void)
{
    HAL_Init();
    UART_init(UART2_ID, 115200);
    SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);
    BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH);
    BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH);
    Systick_add_callback_function(&process_ms);

    Servo_t servos[NUM_SERVOS];

    // Initialize all servos
    for (int i = 0; i < NUM_SERVOS; i++) {
        uint16_t timer = TIMER1_ID + (i / 4); // Distribute across multiple timers if necessary
        uint16_t channel = TIM_CHANNEL_1 + (i % 4) * 4;
        SERVO_init(&servos[i], timer, channel);
    }

    while (1)
    {
        if(!t)
        {
            t = 200; // Refresh rate
            HAL_GPIO_TogglePin(LED_GREEN_GPIO, LED_GREEN_PIN);

            // Example of moving the servos: Modify the duty cycle to move the servos
            for (int i = 0; i < NUM_SERVOS; i++) {
                SERVO_set_position(&servos[i], 50);
            }
        }

        for (int i = 0; i < NUM_SERVOS; i++) {
            SERVO_set_position(&servos[i], 0);
        }
    }
}
