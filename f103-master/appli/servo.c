/**
 * @file servo.c
 *
 *  Created on: 29 févr. 2024
 *      Author: brouzean
 */


#include "config.h"
#include "stm32f1_timer.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "servo.h"

/**
 * @def
 */
#define PERIOD_TIMER 10 //ms

/**
 *
 * @param servo
 */
void SERVO_process_test(Servo_t *servo)
{

	static bool_e previous_button = FALSE;
	bool_e current_button;
	//lecture du bouton bleu
	current_button = !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
	if(current_button && !previous_button) //si appui bouton
	{
		servo->current_position = (servo->current_position > 99)?0:(servo->current_position+5); //de 0 à 100%, par pas de 5%
		SERVO_set_position(servo, servo->current_position);
	}
	previous_button = current_button; //sauvegarde pour le prochain passage
	HAL_Delay(10); //anti-rebond "de fortune" en cadencant la lecture du bouton
}


/**
 *
 * Init Servo_t
 *
 * @param servo
 * @param timer_id_e
 * @param TIM_Chanel
 *
 */
void SERVO_init(Servo_t *servo, timer_id_e timerid, uint16_t TIM_CHANNEL){


	servo->tim_channel = TIM_CHANNEL;
	servo->timerid = timerid;
	servo->current_position = 50;

	//initialisation et lancement du timer1 à une période de 10 ms
	TIMER_run_us(servo->timerid, PERIOD_TIMER*1000, FALSE); //10000us = 10ms
	//activation du signal PWM sur le canal 1 du timer 1 (broche PA8)
	TIMER_enable_PWM(servo->timerid, servo->tim_channel, 150, FALSE, FALSE);
	//rapport cyclique reglé pour une position servo de 50%
	SERVO_set_position(servo, 50);
}


/**
 *
 *	Set new position
 *
 * @param servo
 * @param position est exprimée de 0 à 100.
 *
 */
void SERVO_set_position(Servo_t *servo, uint16_t position){
	servo->current_position = position;
	if(servo->current_position > 100)
		servo->current_position = 100; //écretage si l'utilisateur demande plus de 100%
    uint16_t duty_c = servo->current_position+100;
	TIMER_set_duty(servo->timerid, servo->tim_channel, duty_c );

}


/**
 *
 * Add position to current position
 *
 * @param servo
 * @param position est exprimée de 0 à 100.
 *
 */
void SERVO_increment_position(Servo_t *servo, uint16_t position){
	servo->current_position += position;
	if(servo->current_position > 100)
		servo->current_position = 100; //écretage si l'utilisateur demande plus de 100%
    uint16_t duty_c = servo->current_position+100;
	TIMER_set_duty(servo->timerid, servo->tim_channel, duty_c );

}
