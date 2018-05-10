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
#include "network.h"


#include <paho_mqtt_c/MQTTESP8266.h>
#include <paho_mqtt_c/MQTTClient.h>

void  topic_received(mqtt_message_data_t *md)
{
    uint8_t channel;
    uint32_t duty = 0;
	int i;
    mqtt_message_t *message = md->message;
    printf("Received: ");
    for( i = 0; i < md->topic->lenstring.len; ++i)
        printf("%c", md->topic->lenstring.data[ i ]);

    printf(" = ");
    for( i = 0; i < (int)message->payloadlen; ++i)
        printf("%c", ((char *)(message->payload))[i]);

    printf("\r\n");

    char *data = (char *)md->message->payload;
    data[message->payloadlen] = 0;

    channel =  ((char *)(message->payload))[0] - '0';
    duty =   atoi(++data);

    printf("channel: %x\n", channel);
    printf("duty: %d\n", duty);

    pwm_set_duty(channel,duty);

}



static void  beat_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    char msg[PUB_MSG_LEN];
    int count = 0;

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, 10000 / portTICK_PERIOD_MS);
        printf("beat\r\n");
        snprintf(msg, PUB_MSG_LEN, "Beat %d\r\n", count++);
        if (xQueueSend(publish_queue, (void *)msg, 0) == pdFALSE) {
            printf("Publish queue overflow.\r\n");
        }
    }
}


void user_init(void)
{
    uint8_t pins[6];
    uart_set_baud(0, 115200);

    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    printf("pwm_init(1, [14])\n");
    pins[0] = 16;
    pins[1] = 5;
    pins[2] = 4;
    pins[3] = 0;
    pins[4] = 2;
    pins[5] = 14;
    
    pwm_init(6, pins);

    pwm_set_resolution_freq(100000);
    pwm_set_freq(2000);

    pwm_set_duty(0,100);
    pwm_set_duty(1,200);
    pwm_set_duty(2,300);
    pwm_set_duty(3,600);
	pwm_set_duty(4,800);
	pwm_set_duty(5,1000);

    printf("pwm_start()\n");
    pwm_start();

    vSemaphoreCreateBinary(wifi_alive);
    publish_queue = xQueueCreate(3, PUB_MSG_LEN);

    xTaskCreate(&wifi_task, "wifi_task",  256, NULL, 2, NULL);

    xTaskCreate(&beat_task, "beat_task", 256, NULL, 6, NULL);

    xTaskCreate(&mqtt_task, "mqtt_task", 1024, NULL, 7, NULL);



}
