/* Implementation of PWM support for the Espressif SDK.
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Guillem Pascual Ginovart (https://github.com/gpascualg)
 * Copyright (C) 2015 Javier Cardona (https://github.com/jcard0na)
 * BSD Licensed as described in the file LICENSE
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

static PWMInfo pwmInfo;

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
	static bool val = false;

//	uint32_t load = pwmInfo._maxLoad;
//	timer_set_load(FRC1, load);

	pwmInfo._steps--;
	pwmInfo._stepsOn[0]--;


	if (pwmInfo._steps == 0){

		gpio_write(pwmInfo.pins[1].pin, true);


		if (val == false)
			val = true;
		else
			val = false;

		gpio_write(pwmInfo.pins[0].pin, val);

		pwmInfo._steps = 2000;
//		pwmInfo._stepsOn[0] = pwmInfo.dutyCycles[0];
	}

	if (pwmInfo._stepsOn[0] == 0)
		gpio_write(pwmInfo.pins[1].pin, false);






//    uint8_t i = 0;
//    bool out = true;
//    uint32_t load = pwmInfo._onLoad;
//    pwm_step_t step = PERIOD_ON;
//
//    if (pwmInfo._step == PERIOD_ON)
//    {
//        out = false;
//        load = pwmInfo._offLoad;
//        step = PERIOD_OFF;
//    }
//
//    for (; i < pwmInfo.usedPins; ++i)
//    {
//         gpio_write(pwmInfo.pins[i].pin, pwmInfo.reverse ? !out : out);
//    }
//
//    timer_set_load(FRC1, load);
//    pwmInfo._step = step;
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
        pwmInfo.freq = freq;

        debug("Frequency set at %u",pwmInfo.freq);
    }

}

//void pwm_set_duty(uint16_t duty)
//{
//    pwmInfo.dutyCycle = duty;
//    if (duty == 0 || duty == UINT16_MAX)
//    {
//        pwmInfo.output = (duty == UINT16_MAX);
//    }
//    debug("Duty set at %u",pwmInfo.dutyCycle);
//    pwm_restart();
//}

void pwm_restart()
{
    if (pwmInfo.running)
    {
        pwm_stop();
        pwm_start();
    }
}

void start_timer(){

//	timer_set_load(FRC1, pwmInfo._onLoad);

	timer_set_interrupts(FRC1, true);
	timer_set_run(FRC1, true);

	pwmInfo._steps = 2000;


//	pwmInfo.dutyCycles[0] = 200;
//	pwmInfo._stepsOn[0] = 200;

}

void set_duty(uint8_t channel, uint16_t dutty){


	if (channel > 8 || dutty > 2000)
		return;

//	pwmInfo.dutyCycles[channel] = dutty;

}



//void pwm_start()
//{
//    pwmInfo._onLoad = pwmInfo.dutyCycle * pwmInfo._maxLoad / UINT16_MAX;
//    pwmInfo._offLoad = pwmInfo._maxLoad - pwmInfo._onLoad;
//    pwmInfo._step = PERIOD_ON;
//
//    if(!pwmInfo._onLoad)
//    {
//        debug("Can't set timer with low duty and frequency settings, put duty at 0");
//        pwmInfo.dutyCycle = 0;
//    }
//
//    // 0% and 100% duty cycle are special cases: constant output.
//    if (pwmInfo.dutyCycle > 0 && pwmInfo.dutyCycle < UINT16_MAX)
//    {
//        // Trigger ON
//        uint8_t i = 0;
//        for (; i < pwmInfo.usedPins; ++i)
//        {
//            gpio_write(pwmInfo.pins[i].pin, pwmInfo.reverse ? false : true);
//        }
//        timer_set_load(FRC1, pwmInfo._onLoad);
//        timer_set_reload(FRC1, false);
//        timer_set_interrupts(FRC1, true);
//        timer_set_run(FRC1, true);
//    }
//    else
//    {
//        for (uint8_t i = 0; i < pwmInfo.usedPins; ++i)
//        {
//            gpio_write(pwmInfo.pins[i].pin, pwmInfo.reverse ? !pwmInfo.output : pwmInfo.output );
//        }
//    }
//    debug("PWM started");
//    pwmInfo.running = 1;
//}

void pwm_stop()
{
    timer_set_interrupts(FRC1, false);
    timer_set_run(FRC1, false);
    for (uint8_t i = 0; i < pwmInfo.usedPins; ++i)
    {
//        gpio_write(pwmInfo.pins[i].pin, pwmInfo.reverse ? true : false);
    }       
    debug("PWM stopped");
    pwmInfo.running = 0;
}
