/* Implementation of PWM support for the ESP with open-rtos SDK.
 *
 * Based example from esp-open-rtos
 * Copyright (C) 2018 Renan Augusto Starke (https://github.com/xtarke)
 */

#ifndef SOFT_PWM_H_
#define SOFT_PWM_H_

#include <stdint.h>

#define MAX_PWM_PINS    8

#ifdef __cplusplus
extern "C" {
#endif

//Warning: Printf disturb pwm. You can use "uart_putc" instead.

/**
 * Initialize pwm
 * @param npins Number of pwm pin used
 * @param pins Array pointer to the pins
 * @param reverse If true, the pwm work in reverse mode
 */    
void pwm_init(uint8_t npins, const uint8_t* pins);


/**
 * Set PWM resolution frequency. If error, frequency not set
 * @param freq PWM frequency value in Hertz
 */
void pwm_set_resolution_freq(uint32_t freq);

/**
 * Set PWM frequency. If error, frequency not set
 * @param freq_steps PWM frequency value in steps of period set in
 * pwm_set_resolution_freq().
 *
 * Example: if pwm_set_resolution_freq is 100kHz, T is 10us
 *          2000 freq_steps generates a 50Hz PWM with 10us resolution
 */
void pwm_set_freq(uint32_t freq_steps);

/**
 * Set Duty between 0 and freq_steps
 * @param channel
 * @param duty Duty value
 */  
void pwm_set_duty(uint8_t channel, uint32_t duty);

/**
 * Restart the pwm signal
 */  
void pwm_restart();

/**
 * Start the pwm signal
 */  
void pwm_start();

/**
 * Stop the pwm signal
 */  
void pwm_stop();

#ifdef __cplusplus
}
#endif

#endif /* SOFT_PWM_H_ */
