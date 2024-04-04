/*
 * servo.c
 *
 *  Created on: 29 févr. 2024
 *      Author: brouzean
 */
#include "config.h"
#include "stm32f1_timer.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "servo.h"

#define PERIOD_TIMER 10 //ms

static uint16_t current_position = 50;

/**
 *
 */
void SERVO_process_test(void)
{

	static bool_e previous_button = FALSE;
	bool_e current_button;
	//lecture du bouton bleu
	current_button = !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
	if(current_button && !previous_button) //si appui bouton
	{
		current_position = (current_position > 99)?0:(current_position+5); //de 0 à 100%, par pas de 5%
		SERVO_set_position(current_position);
	}
	previous_button = current_button; //sauvegarde pour le prochain passage
	HAL_Delay(10); //anti-rebond "de fortune" en cadencant la lecture du bouton
}


/**
 *
 */
void SERVO_init(timer_id_e timerid, uint32_t TIM_CHANNEL){
	//initialisation et lancement du timer1 à une période de 10 ms
	TIMER_run_us(timerid, PERIOD_TIMER*1000, FALSE); //10000us = 10ms
	//activation du signal PWM sur le canal 1 du timer 1 (broche PA8)
	TIMER_enable_PWM(timerid, TIM_CHANNEL, 150, FALSE, FALSE);
	//rapport cyclique reglé pour une position servo de 50%
	SERVO_set_position(50);
}


/**
 * @argument position est exprimée de 0 à 100.
 *
 */
void SERVO_set_position(uint16_t position){
	current_position = position;
	if(current_position > 100)
		current_position = 100; //écretage si l'utilisateur demande plus de 100%
    uint16_t duty_c = current_position+100;
	TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_1, duty_c );

	//TODO : mise à jour du rapport cyclique.
	//duty doit être exprimé ici de 100 à 200 (sur 1000) (pour un rapport cyclique de 10% à 20%, c’est-à-dire une durée de pulse de 1 à 2ms dans la période de 10ms)
	//Donc on additionne 100 à position.
}

/**
 *
 */
uint16_t SERVO_get_position(void) {
	return current_position;
}

