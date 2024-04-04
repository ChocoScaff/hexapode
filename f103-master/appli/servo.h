/*
 * servo.h
 *
 *  Created on: 29 févr. 2024
 *      Author: brouzean
 */

#ifndef SERVO_H_
#define SERVO_H_

void SERVO_process_test(void);
void SERVO_init(timer_id_e timerid, uint32_t TIM_CHANNEL);
void SERVO_set_position(uint16_t position);
uint16_t SERVO_get_position(void);
#endif /* SERVO_H_ */
