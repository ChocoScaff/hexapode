/*
 * stm32f1_pwm.c
 *
 *  Created on: 23 mai 2016
 *      Author: Spoiraud
 */
#include "config.h"
#if USE_PWM
#include "stm32f1_pwm.h"
#include "stm32f1_timer.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"


void PWM_test(void)
{
	PWM_run(TIMER4_ID, TIM_CHANNEL_3, FALSE, 1, 50, FALSE);		//Timer 4 - Canal 3 Positive (4.CH1)  -> PB8

	PWM_run(TIMER3_ID, TIM_CHANNEL_1, FALSE,1000, 50, FALSE);	//Timer 3 - Canal 1 Positive (3.CH1)  -> PA6

	PWM_run(TIMER1_ID, TIM_CHANNEL_1, FALSE,1000, 1, FALSE);	//Timer 1 - Canal 1 Positive (1.CH1)  -> PA8
	PWM_run(TIMER1_ID, TIM_CHANNEL_2, FALSE,1000, 1, FALSE);	//Timer 1 - Canal 2 Positive (1.CH2)  -> PA9
	PWM_run(TIMER1_ID, TIM_CHANNEL_1, TRUE,1000, 10, FALSE);	//Timer 1 - Canal 1 Negative (1.CH1N) -> PB13
	PWM_run(TIMER1_ID, TIM_CHANNEL_2, TRUE,1000, 10, FALSE);	//Timer 1 - Canal 2 Negative (1.CH2N) -> PB14

	//exemple pour un servomoteur...
	//cet exemple �crase les configs pr�c�dentes du timer 1 (car la p�riode change)
	PWM_run(TIMER1_ID, TIM_CHANNEL_3, FALSE,20000, 5, FALSE);	//Timer 1 - Canal 3 Positive (1.CH3) -> PA10

}

/** @brief 	Fonction qui configure et lance la pwm demand�e
 * @func 	void PWM_run(timer_id_e timer_id, uint16_t TIM_CHANNEL_x, uint32_t period_us, uint32_t duty, bool_e negative_chanel)
 * @param	timer_id : TIMERX_ID, o� X est le num�ro du timer
 * @param	TIM_CHANNEL_x, o� x est le num�ro du canal
 * @param	negative channel : TRUE s'il s'agit de configuer l'un des canaux CH1_N, CH2_N, CH3_N. False sinon.
 * @param	period_us : p�riode demand�e pour le signal pwm, en microsecondes, de 1us � 40s
 * @param	duty : rapport cyclique souhait�. ATTENTION : exprim� en pourcents. (de 0 � 100).
 * @param   remap : TRUE pour activer le remappage de la broche, c'est � dire TRUE si le petit 'r' dans le ficher excel pr�sentant les ports du microcontr�lleur est pr�sent dans la colonne du timer pour le channel associ�� la broche du micro, pour la bluepill ou nucleo
 * @post	Premi�rement, cette fonction configure la broche correspondante au canal demand�.
 * 			Ensuite, cette fonction lance (ou relance !) le timer demand� avec la p�riode demand�e.
 * 			Puis, elle lance un signal PWM au rapport cyclique demand�.
 * @pre		Attention, si vous utilisez cette fonction pour plusieurs canaux d'un m�me timer, veillez � indiquer les m�mes valeurs de period_us ! Sinon, la derni�re valeur gagnera !
 *
 */
void PWM_run(timer_id_e timer_id, uint32_t TIM_CHANNEL_x, bool_e negative_channel, uint32_t period_us, uint32_t duty, bool_e remap)
{
	if(!IS_TIM_CHANNELS(TIM_CHANNEL_x) || timer_id >= TIMER_ID_NB || period_us > 40000000 || duty > 100)
		return;

	PWM_configure_pin(timer_id, TIM_CHANNEL_x, negative_channel, TRUE, remap);

	TIM_HandleTypeDef * handler = NULL;
	TIMER_run_us(timer_id, period_us, FALSE);
	handler = TIMER_get_phandler(timer_id);
	if(handler != NULL)
	{
		TIM_OC_InitTypeDef TIM_OCInitStructure;
		TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
		TIM_OCInitStructure.Pulse = 0;
		TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
		TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE; //disable the fast state
		TIM_OCInitStructure.OCNIdleState = 0;
		TIM_OCInitStructure.OCIdleState = 0;
		HAL_TIM_OC_Init(handler);
		HAL_TIM_OC_ConfigChannel(handler, &TIM_OCInitStructure, TIM_CHANNEL_x); //on configure le canal (avant on autorisait le prechargement de la config)
		if(negative_channel)
			HAL_TIMEx_PWMN_Start(handler, TIM_CHANNEL_x);
		else
			HAL_TIM_PWM_Start(handler, TIM_CHANNEL_x);

		PWM_set_duty(timer_id, TIM_CHANNEL_x, duty);
	}
}

/** @brief	fonction qui r�gle le rapport cyclique sur le canal demand�.
 * @func	PWM_set_duty(timer_id_e timer_id, uint16_t TIM_CHANNEL_x, uint16_t duty)
 * @param	timer_id : TIMERX_ID, o� X est le num�ro du timer
 * @param	TIM_CHANNEL_x, o� x est le num�ro du canal
 * @param	duty : rapport cyclique exprim� en pourcents (de 0 � 100)
 * @post	Des effets de bords (arrondis) peuvent appara�tre si la periode du timer n'est pas un multiple de 100.
 * 			Attention, m�me si la p�riode du timer est multiple de 100us, cela peut se produire si la fonction TIMER_run_us a calcul� une p�riode non multiple de 100 !
 */
void PWM_set_duty(timer_id_e timer_id, uint32_t TIM_CHANNEL_x, uint32_t duty)
{
	TIM_HandleTypeDef * handler = NULL;
	handler = TIMER_get_phandler(timer_id);
	duty = MIN(100,duty);	//Ecretage... Le rapport cyclique ne peut donc pas �tre plus grand que 100 !
	duty = ((duty*(handler->Init.Period+1))/100);

	__HAL_TIM_SET_COMPARE(handler, TIM_CHANNEL_x, duty);
}

void PWM_set_prescaler(timer_id_e timer_id, uint32_t prescaler)
{
	TIM_HandleTypeDef * handler = NULL;
	handler = TIMER_get_phandler(timer_id);
	handler->Init.Prescaler = prescaler - 1;
	__HAL_TIM_SET_PRESCALER(handler, prescaler - 1);
}


void PWM_set_period_and_duty(timer_id_e timer_id, uint32_t TIM_CHANNEL_x, uint32_t period_us, uint32_t duty)
{
	TIM_HandleTypeDef * handler = NULL;
	handler = TIMER_get_phandler(timer_id);
	duty = MIN(100,duty);	//Ecretage... Le rapport cyclique ne peut donc pas �tre plus grand que 100 !

	__HAL_TIM_SET_AUTORELOAD(handler, period_us - 1);

	duty = ((duty*(handler->Init.Period+1))/100);
	__HAL_TIM_SET_COMPARE(handler, TIM_CHANNEL_x, duty);
}


/*
 * @brief	Cette fonction configure la broche PWM du canal demand�.
 * @func 	void PWM_configure_pin(timer_id_e timer_id, uint16_t TIM_CHANNEL_x, bool_e negative_chanel, bool_e enable, bool_e remap)
 * @param	timer_id : TIMERX_ID, o� X est le num�ro du timer
 * @param	TIM_CHANNEL_x, o� x est le num�ro du canal
 * @param	negative channel : TRUE s'il s'agit de configuer l'un des canaux CH1_N, CH2_N, CH3_N. False sinon.
 * @param	enable : TRUE pour configurer la broche en sortie push-pull et faire sortir le signal PWM. FALSE pour configurer la broche en entr�e.
 * @param   remap : TRUE pour activer le remappage de la broche, c'est � dire TRUE si le petit 'r' dans le ficher excel pr�sentant les ports du microcontr�lleur est pr�sent dans la colonne du timer pour le channel associ�� la broche du micro pour la bluepill ou nucleo
 **/
void PWM_configure_pin(timer_id_e timer_id, uint16_t TIM_CHANNEL_x, bool_e negative_channel, bool_e enable, bool_e remap)
{
	if(!IS_TIM_CHANNELS(TIM_CHANNEL_x) || timer_id >= TIMER_ID_NB)
		return;
	uint32_t mode, pull;
	if(enable)
	{
		mode = GPIO_MODE_AF_PP;
		pull = GPIO_NOPULL;
	}
	else
	{
		mode = GPIO_MODE_INPUT;
		pull = GPIO_PULLDOWN;
	}

	//On configure la broche indiqu�e pour le mode AF (Alternate Function), associ�e au p�rih�rique TIMx.
	//Cette broche ne sera donc plus contr�lable comme une entr�e sortie � usage g�n�ral (GPIO)... On l'a confi�e au timer TIMx.
	if(negative_channel)
	{
		if(timer_id == TIMER1_ID)
		{
			switch(TIM_CHANNEL_x)
			{
				case TIM_CHANNEL_1:	if (remap) {
										BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_7, mode, pull, GPIO_SPEED_FREQ_HIGH);
										__HAL_AFIO_REMAP_TIM1_PARTIAL();
									}
									else{
										BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_13, mode, pull, GPIO_SPEED_FREQ_HIGH);
									}
									break;
				case TIM_CHANNEL_2:	if (remap) {
										BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_0, mode, pull, GPIO_SPEED_FREQ_HIGH);
										__HAL_AFIO_REMAP_TIM1_PARTIAL();
									}
									else{
										BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_14, mode, pull, GPIO_SPEED_FREQ_HIGH);
									}
									break;
				case TIM_CHANNEL_3: if(remap) {
										BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_1, mode, pull, GPIO_SPEED_FREQ_HIGH);
										__HAL_AFIO_REMAP_TIM1_PARTIAL();
									}
									else{
										BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_15, mode, pull, GPIO_SPEED_FREQ_HIGH);
									}
									break;
				default:	break;
			}
		}
	}
	else
	{
		switch(timer_id)
		{
			case TIMER1_ID:
				switch(TIM_CHANNEL_x)
				{
					case TIM_CHANNEL_1:	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_8,  mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					case TIM_CHANNEL_2:	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_9,  mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					case TIM_CHANNEL_3:	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_10, mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					case TIM_CHANNEL_4:	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_11, mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					default:	break;
				}
				break;
			case TIMER2_ID:
				switch(TIM_CHANNEL_x)
				{
					case TIM_CHANNEL_1:	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_0,  mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					case TIM_CHANNEL_2:	if (remap){
											BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_3,  mode, pull, GPIO_SPEED_FREQ_HIGH);
											__HAL_AFIO_REMAP_TIM2_PARTIAL_1();
										}
										else{
											BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_1,  mode, pull, GPIO_SPEED_FREQ_HIGH);
										}
										break;
					case TIM_CHANNEL_3:	if(remap){
											BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_10,  mode, pull, GPIO_SPEED_FREQ_HIGH);
											__HAL_AFIO_REMAP_TIM2_PARTIAL_2();
										}
										else {
											BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_2,  mode, pull, GPIO_SPEED_FREQ_HIGH);
										}
										break;
					case TIM_CHANNEL_4:	if (remap){
											BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_11,  mode, pull, GPIO_SPEED_FREQ_HIGH);
											__HAL_AFIO_REMAP_TIM2_PARTIAL_2();
										}
										else{
											BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_3,  mode, pull, GPIO_SPEED_FREQ_HIGH);
										}
										break;
					default:	break;
				}
				break;
			case TIMER3_ID:
				switch(TIM_CHANNEL_x)
				{
					case TIM_CHANNEL_1:	if(remap){
											BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_6,  mode, pull, GPIO_SPEED_FREQ_HIGH);
											__HAL_AFIO_REMAP_TIM3_ENABLE();
										}
										else {
											BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_6,  mode, pull, GPIO_SPEED_FREQ_HIGH);
										}
										break;

					case TIM_CHANNEL_2:	if(remap){
											BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_7,  mode, pull, GPIO_SPEED_FREQ_HIGH);
											__HAL_AFIO_REMAP_TIM3_ENABLE();
										}
										else {
											BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_7,  mode, pull, GPIO_SPEED_FREQ_HIGH);
										}
										break;

					case TIM_CHANNEL_3:	if(remap){
											BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_8,  mode, pull, GPIO_SPEED_FREQ_HIGH);
											__HAL_AFIO_REMAP_TIM3_ENABLE();
										}
										else {
											BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_0,  mode, pull, GPIO_SPEED_FREQ_HIGH);
										}
										break;

					case TIM_CHANNEL_4:	if(remap){
											BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_9,  mode, pull, GPIO_SPEED_FREQ_HIGH);
											__HAL_AFIO_REMAP_TIM3_ENABLE();
										}
										else {
											BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_1,  mode, pull, GPIO_SPEED_FREQ_HIGH);
										}
										break;
					default:	break;
				}
				break;
			case TIMER4_ID:
				switch(TIM_CHANNEL_x)
				{
					case TIM_CHANNEL_1:	BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_6,  mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					case TIM_CHANNEL_2:	BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_7,  mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					case TIM_CHANNEL_3:	BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_8,  mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					case TIM_CHANNEL_4:	BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_9,  mode, pull, GPIO_SPEED_FREQ_HIGH);	break;
					default:	break;
				}
				break;
				break;
			default:
				break;
		}
	}

}

#endif
