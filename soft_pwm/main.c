/* Very basic example to test the pwm library
 * Hook up an LED to pin14 and you should see the intensity change
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Javier Cardona (https://github.com/jcard0na)
 * BSD Licensed as described in the file LICENSE
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "soft_pwm.h"

void task1(void *pvParameters)
{
    printf("Hello from task1!\r\n");
    uint32_t const init_count = 0;
    uint32_t count = init_count;

    uint16_t dutty = 20;

    while(1) {
        vTaskDelay(100);
        //printf("duty cycle set to %d/UINT16_MAX%%\r\n", count);
        //pwm_set_duty(count);

        set_duty(0, dutty);

        dutty += dutty + 200;


        if (dutty > 2000)
        	dutty = 20;


        count += UINT16_MAX/17;
        if (count > UINT16_MAX)
            count = init_count;
    }
}

void user_init(void)
{
    uint8_t pins[2];
    uart_set_baud(0, 115200);

    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    printf("pwm_init(1, [14])\n");
    pins[0] = 14;
    pins[1] = 12;
    
    pwm_init(2, pins);

    printf("pwm_set_freq(1000)     # 1 kHz\n");

    pwm_set_resolution_freq(100000);

    //pwm_set_freq(100000);
    //pwm_set_freq(500);
   
    
  //  start_timer();


    printf("pwm_set_duty(UINT16_MAX/2)     # 50%%\n");
    //pwm_set_duty(UINT16_MAX/2);

    printf("pwm_start()\n");
    //pwm_start();

    xTaskCreate(task1, "tsk1", 256, NULL, 2, NULL);
}
