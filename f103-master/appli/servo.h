/*
 * servo.h
 *
 *  Created on: 29 févr. 2024
 *      Author: brouzean
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "stm32f1xx_hal_tim.h"
#include "stm32f1_timer.h"

typedef struct {
	timer_id_e timerid;
	uint16_t tim_channel;
	uint16_t current_position;
}Servo_t;


void SERVO_process_test(Servo_t *servo);
void SERVO_init(Servo_t *servo, timer_id_e timerid, uint16_t TIM_CHANNEL);
void SERVO_set_position(Servo_t *servo, uint16_t position);
#endif /* SERVO_H_ */

