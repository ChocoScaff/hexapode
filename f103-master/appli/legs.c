void SetServoAngle(TIM_HandleTypeDef *htim, uint32_t Channel, uint8_t pin, uint8_t value) {
    // Assuming a 20ms period (50Hz) for the servo, and a timer clock of 72MHz.
    // The prescaler and period might need adjustment depending on your timer configuration.
    // The calculation also depends on your servo's minimum and maximum pulse widths (1ms for 0 degrees, 2ms for 180 degrees is common).

    uint32_t pulseWidth, minPulseWidth = 1000, maxPulseWidth = 2000; // Pulse widths in microseconds
    uint32_t timerFrequency = 72000000; // Timer frequency in Hz (72MHz for STM32F103)
    uint32_t prescaler = htim->Init.Prescaler + 1; // Prescaler value used in timer initialization (+1 because prescaler starts from 0)
    uint32_t period = htim->Init.Period + 1; // Period value used in timer initialization (+1 because period counts from 0)
    uint32_t timerClock = timerFrequency / prescaler; // Effective timer clock after prescaling
    
    // Convert desired angle (0-180 degrees) to pulse width in microseconds
    pulseWidth = ((value * (maxPulseWidth - minPulseWidth)) / 180) + minPulseWidth;

    // Convert pulse width in microseconds to timer ticks
    uint32_t pulseLengthTicks = (pulseWidth * timerClock) / 1000000;

    // Make sure the pulse length is within the timer's period
    if (pulseLengthTicks >= period) {
        pulseLengthTicks = period - 1;
    }

    // Update the PWM pulse width
    __HAL_TIM_SET_COMPARE(htim, Channel, pulseLengthTicks);
}