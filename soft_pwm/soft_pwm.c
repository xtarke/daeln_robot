/* Implementation of PWM support for the ESP with open-rtos SDK.
 *
 * Based example from esp-open-rtos
 * Copyright (C) 2018 Renan Augusto Starke (https://github.com/xtarke)
 */
#include "soft_pwm.h"

#include <espressif/esp_common.h>
#include <espressif/sdk_private.h>
#include <FreeRTOS.h>
#include <esp8266.h>

#define PWM_DEBUG


#ifdef PWM_DEBUG
#define debug(fmt, ...) printf("%s: " fmt "\n", "PWM", ## __VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

typedef struct PWMPinDefinition
{
    uint8_t pin;
    uint8_t divider;
} PWMPin;

typedef enum {
    PERIOD_ON   = 0,
    PERIOD_OFF  = 1
} pwm_step_t;


typedef struct pwmInfoDefinition
{
    uint8_t running;

    uint32_t timerIncfreq;
    uint32_t freq;

    uint32_t stepsOn[8];

    /* private */
    uint32_t _steps;
    uint32_t _stepsOn[8];

    uint16_t usedPins;
    PWMPin pins[8];
} PWMInfo;

volatile static PWMInfo pwmInfo;

static void IRAM frc1_interrupt_handler(void *arg);

void pwm_init(uint8_t npins, const uint8_t* pins)
{
	 uint8_t i;

    /* Assert number of pins is correct */
    if (npins > MAX_PWM_PINS)
    {
        debug("Incorrect number of PWM pins (%d)\n", npins);
        return;
    }

    /* Initialize */
    pwmInfo._steps = 0;
    /* Save pins information */
    pwmInfo.usedPins = npins;

    for (i=0; i < npins; ++i)
    {
        pwmInfo.pins[i].pin = pins[i];
        pwmInfo._stepsOn[i] = 0;
        pwmInfo.stepsOn[i] = 0;

        /* configure GPIOs */
        gpio_enable(pins[i], GPIO_OUTPUT);
    }

    /* Stop timers and mask interrupts */
    pwm_stop();

    /* set up ISRs */
    _xt_isr_attach(INUM_TIMER_FRC1, frc1_interrupt_handler, NULL);

    /* Flag not running */
    pwmInfo.running = 0;
    debug("PWM Init");
}



static void IRAM frc1_interrupt_handler(void *arg)
{
	int i;
	/* Reprogram next IRQ */
	timer_set_load(FRC1, pwmInfo.timerIncfreq);

	/* Base frequency */
	pwmInfo._steps--;

	/* Next PWM period */
	if (pwmInfo._steps == 0){

		/* Next period turn on all used pins */
		for (uint8_t i = 0; i < pwmInfo.usedPins; ++i){
			if (pwmInfo.stepsOn[i] > 0) {
				gpio_write(pwmInfo.pins[i].pin, true);
				pwmInfo._stepsOn[i] = pwmInfo.stepsOn[i];
			}
		}
		pwmInfo._steps = pwmInfo.freq;
	}

	/* Period ON */
	for (i=0; i < pwmInfo.usedPins; ++i){
		pwmInfo._stepsOn[i]--;

		if (!pwmInfo._stepsOn[i])
			gpio_write(pwmInfo.pins[i].pin, false);
	}
}


void pwm_set_resolution_freq(uint32_t freq)
{
    /* Stop now to avoid load being used */
    if (pwmInfo.running)
    {
        pwm_stop();
        pwmInfo.running = 1;
    }

    if (!timer_set_frequency(FRC1, freq))
    {
        pwmInfo.timerIncfreq = timer_get_load(FRC1);

        debug("PWM resolution set at %u", freq);
    }
}

void pwm_set_freq(uint32_t freq_steps)
{
    /* Stop now to avoid load being used */
    if (pwmInfo.running)
    {
        pwm_stop();
        pwmInfo.running = 1;
    }

    pwmInfo.freq = freq_steps;
}

void pwm_restart()
{
    if (pwmInfo.running)
    {
        pwm_stop();
        pwm_start();
    }
}

void pwm_set_duty(uint8_t channel, uint32_t dutty){

	if (channel > pwmInfo.usedPins || dutty > pwmInfo.freq)
		return;

	pwmInfo.stepsOn[channel] = dutty;
}

void pwm_start()
{

	if (pwmInfo.running)
		return;

	/* Base PWM frequency */
	pwmInfo._steps = pwmInfo.freq;

	/* Set duty cycle for all channels */
	for (uint8_t i = 0; i < pwmInfo.usedPins; ++i)
		pwmInfo._stepsOn[i] = pwmInfo.stepsOn[i];

	/* Enable Timer */
	timer_set_load(FRC1, pwmInfo.timerIncfreq);
	timer_set_reload(FRC1, false);
	timer_set_interrupts(FRC1, true);
	timer_set_run(FRC1, true);

	pwmInfo.running = 1;

}

void pwm_stop()
{
    timer_set_interrupts(FRC1, false);
    timer_set_run(FRC1, false);

    for (uint8_t i = 0; i < pwmInfo.usedPins; ++i)
        gpio_write(pwmInfo.pins[i].pin, false);

    debug("PWM stopped");
    pwmInfo.running = 0;
}
