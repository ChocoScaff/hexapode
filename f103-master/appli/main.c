/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/
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


int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la premi�re �tape de la fonction main().
	HAL_Init();

	//Initialisation de l'UART2 � la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reli�es jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirig�es vers la sonde de d�bogage, la liaison UART �tant ensuite encapsul�e sur l'USB vers le PC de d�veloppement.
	UART_init(UART2_ID,115200);

	//"Indique que les printf sortent vers le p�riph�rique UART2."
	SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//On ajoute la fonction process_ms � la liste des fonctions appel�es automatiquement chaque ms par la routine d'interruption du p�riph�rique SYSTICK
	Systick_add_callback_function(&process_ms);

	Servo_t servo1;
    Servo_t servo2;
    Servo_t servo3;
    Servo_t servo4;
    Servo_t servo5;
    Servo_t servo6;
    Servo_t servo7;
    Servo_t servo8;
    Servo_t servo9;
    Servo_t servo10;
    Servo_t servo11;
    Servo_t servo12;

	SERVO_init(&servo1, TIMER1_ID, TIM_CHANNEL_1);
    SERVO_init(&servo2, TIMER1_ID, TIM_CHANNEL_2);
    SERVO_init(&servo3, TIMER1_ID, TIM_CHANNEL_3);
    SERVO_init(&servo4, TIMER1_ID, TIM_CHANNEL_4);
    SERVO_init(&servo5, TIMER2_ID, TIM_CHANNEL_1);
    SERVO_init(&servo6, TIMER2_ID, TIM_CHANNEL_2);
    SERVO_init(&servo7, TIMER2_ID, TIM_CHANNEL_3);
    SERVO_init(&servo8, TIMER2_ID, TIM_CHANNEL_4);
    SERVO_init(&servo9, TIMER3_ID, TIM_CHANNEL_1);
    SERVO_init(&servo10, TIMER3_ID, TIM_CHANNEL_2);
    SERVO_init(&servo11, TIMER3_ID, TIM_CHANNEL_3);
    SERVO_init(&servo12, TIMER3_ID, TIM_CHANNEL_4);

    while(1)
    {
        if(!t)
        {
            t = 200; // Refresh rate
            HAL_GPIO_TogglePin(LED_GREEN_GPIO, LED_GREEN_PIN);

            // Example of moving the servo: Modify the duty cycle to move the servo
            // Here, just toggling between two positions as an example
            SERVO_set_position(&servo1, 50);
            SERVO_set_position(&servo2, 50);
            SERVO_set_position(&servo3, 50);
            SERVO_set_position(&servo4, 50);
            SERVO_set_position(&servo5, 50);
            SERVO_set_position(&servo6, 50);
            SERVO_set_position(&servo7, 50);
            SERVO_set_position(&servo8, 50);
            SERVO_set_position(&servo9, 50);
            SERVO_set_position(&servo10, 50);
            SERVO_set_position(&servo11, 50);
            SERVO_set_position(&servo12, 50);
            )
        }
        SERVO_set_position(&servo1, 0);

    }
}
