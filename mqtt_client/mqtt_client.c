#include "espressif/esp_common.h"
#include "esp/uart.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <ssid_config.h>

#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>

#include <paho_mqtt_c/MQTTESP8266.h>
#include <paho_mqtt_c/MQTTClient.h>

#include <semphr.h>

#include "comm.h"

/* You can use http://test.mosquitto.org/ to test mqtt_client instead
 * of setting up your own MQTT server */
#define MQTT_HOST ("192.168.25.71")
#define MQTT_PORT 1883

#define MQTT_USER NULL
#define MQTT_PASS NULL

SemaphoreHandle_t wifi_alive;
QueueHandle_t publish_queue;
QueueHandle_t tx_queue;
QueueHandle_t rx_queue;

static TaskHandle_t xHandlingUartTask;
static TaskHandle_t xHandlingPkgTask;

#define PUB_MSG_LEN 16

static void  beat_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    char msg[PUB_MSG_LEN];
    int count = 0;

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, 10000 / portTICK_PERIOD_MS);
        //printf("beat\r\n");
        
        snprintf(msg, PUB_MSG_LEN, "Beat %d\r\n", count++);
        if (xQueueSend(publish_queue, (void *)msg, 0) == pdFALSE) {
            printf("Publish queue overflow.\r\n");
        }
    }
}

static void  topic_received(mqtt_message_data_t *md)
{
    int i;
    mqtt_message_t *message = md->message;
    printf("Received: ");
    for( i = 0; i < md->topic->lenstring.len; ++i)
        printf("%c", md->topic->lenstring.data[ i ]);

    printf(" = ");
    for( i = 0; i < (int)message->payloadlen; ++i)
        printf("%c", ((char *)(message->payload))[i]);

    printf("\r\n");
}

static const char *  get_my_id(void)
{
    // Use MAC address for Station as unique ID
    static char my_id[13];
    static bool my_id_done = false;
    int8_t i;
    uint8_t x;
    if (my_id_done)
        return my_id;
    if (!sdk_wifi_get_macaddr(STATION_IF, (uint8_t *)my_id))
        return NULL;
    for (i = 5; i >= 0; --i)
    {
        x = my_id[i] & 0x0F;
        if (x > 9) x += 7;
        my_id[i * 2 + 1] = x + '0';
        x = my_id[i] >> 4;
        if (x > 9) x += 7;
        my_id[i * 2] = x + '0';
    }
    my_id[12] = '\0';
    my_id_done = true;
    return my_id;
}

static void  mqtt_task(void *pvParameters)
{
    int ret         = 0;
    struct mqtt_network network;
    mqtt_client_t client   = mqtt_client_default;
    char mqtt_client_id[20];
    uint8_t mqtt_buf[100];
    uint8_t mqtt_readbuf[100];
    mqtt_packet_connect_data_t data = mqtt_packet_connect_data_initializer;

    mqtt_network_new( &network );
    memset(mqtt_client_id, 0, sizeof(mqtt_client_id));
    strcpy(mqtt_client_id, "ESP-");
    strcat(mqtt_client_id, get_my_id());

    while(1) {
        xSemaphoreTake(wifi_alive, portMAX_DELAY);
        printf("%s: started\n\r", __func__);
        printf("%s: (Re)connecting to MQTT server %s ... ",__func__,
               MQTT_HOST);
        ret = mqtt_network_connect(&network, MQTT_HOST, MQTT_PORT);
        if( ret ){
            printf("error: %d\n\r", ret);
            taskYIELD();
            continue;
        }
        printf("done\n\r");
        mqtt_client_new(&client, &network, 5000, mqtt_buf, 100,
                      mqtt_readbuf, 100);

        data.willFlag       = 0;
        data.MQTTVersion    = 3;
        data.clientID.cstring   = mqtt_client_id;
        data.username.cstring   = MQTT_USER;
        data.password.cstring   = MQTT_PASS;
        data.keepAliveInterval  = 10;
        data.cleansession   = 0;
        printf("Send MQTT connect ... ");
        ret = mqtt_connect(&client, &data);
        if(ret){
            printf("error: %d\n\r", ret);
            mqtt_network_disconnect(&network);
            taskYIELD();
            continue;
        }
        printf("done\r\n");
        mqtt_subscribe(&client, "/esptopic", MQTT_QOS1, topic_received);
        xQueueReset(publish_queue);

        while(1){

            char msg[PUB_MSG_LEN - 1] = "\0";
            while(xQueueReceive(publish_queue, (void *)msg, 0) ==
                  pdTRUE){
                printf("got message to publish\r\n");
                mqtt_message_t message;
                message.payload = msg;
                message.payloadlen = PUB_MSG_LEN;
                message.dup = 0;
                message.qos = MQTT_QOS1;
                message.retained = 0;
                ret = mqtt_publish(&client, "/beat", &message);
                if (ret != MQTT_SUCCESS ){
                    printf("error while publishing message: %d\n", ret );
                    break;
                }

			 }

            ret = mqtt_yield(&client, 1000);
            if (ret == MQTT_DISCONNECTED)
                break;
        }
        printf("Connection dropped, request restart\n\r");
        mqtt_network_disconnect(&network);
        taskYIELD();
    }
}

static void  wifi_task(void *pvParameters)
{
    uint8_t status  = 0;
    uint8_t retries = 30;
    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };

    printf("WiFi: connecting to WiFi\n\r");
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    while(1)
    {
        while ((status != STATION_GOT_IP) && (retries)){
            status = sdk_wifi_station_get_connect_status();
            printf("%s: status = %d\n\r", __func__, status );
            if( status == STATION_WRONG_PASSWORD ){
                printf("WiFi: wrong password\n\r");
                break;
            } else if( status == STATION_NO_AP_FOUND ) {
                printf("WiFi: AP not found\n\r");
                break;
            } else if( status == STATION_CONNECT_FAIL ) {
                printf("WiFi: connection failed\r\n");
                break;
            }
            vTaskDelay( 1000 / portTICK_PERIOD_MS );
            --retries;
        }
        if (status == STATION_GOT_IP) {
            printf("WiFi: Connected\n\r");
            xSemaphoreGive( wifi_alive );
            taskYIELD();
        }

        while ((status = sdk_wifi_station_get_connect_status()) == STATION_GOT_IP) {
            xSemaphoreGive( wifi_alive );
            taskYIELD();
        }
        printf("WiFi: disconnected\n\r");
        sdk_wifi_station_disconnect();
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}

static void  status_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint8_t pkg[PKG_MAX_SIZE] = {0,0,0x13};
    int count = 0;

    xSemaphoreTake(wifi_alive, portMAX_DELAY);

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, 5000 / portTICK_PERIOD_MS);

        xTaskNotify( xHandlingUartTask, 0, eNoAction );

        if (xQueueSend(tx_queue, (void *)pkg, 0) == pdFALSE) {
            printf("uart_queue overflow.\r\n");
        }
    }
}

static void  uart_task(void *pvParameters){

	BaseType_t xResult;

	uint8_t pkg[PKG_MAX_SIZE] = { 0, 0, 0x13, 0, 0};
	uint8_t i;
	uint8_t size;
	uint8_t retries;

	uint32_t ulNotifiedValue;

	for (;;){
		/* Wait for wifi */
		//xSemaphoreTake(wifi_alive, portMAX_DELAY);

		xResult = xTaskNotifyWait( pdFALSE,          /* Don't clear bits on entry. */
		                           ULONG_MAX,        /* Clear all bits on exit. */
		                           &ulNotifiedValue, /* Stores the notified value. */
								   portMAX_DELAY);

		printf("got uart message\r\n");


		while(xQueueReceive(tx_queue, (void *)pkg, 0) ==
		                  pdTRUE){
		 	/* Send a package */
			makeAndSend(pkg, 1);

			printf("Sent\n\r");

			retries = 30;
			size = 0;

			/* Wait for response: at least 2 bytes (header) */
			while (size < 2 && (retries)){
				vTaskDelay( 200 / portTICK_PERIOD_MS );
				size = uart_rxfifo_size(0);
				retries--;
			}

			/* No response, try to send again */
			if (!retries){
				vTaskDelay( 2000 / portTICK_PERIOD_MS );
				uart_flush_rxfifo(0);
				printf("Uart task: no response\n\r");
				break;
			}

			/* Get received data */
			for (i=0; (i < 2) & (i < PKG_MAX_SIZE); i++)
				pkg[i] = uart_getc_nowait(0);

			/* Check for a valid start byte */
			if (pkg[0] != PKG_START){
				 taskYIELD();
				 //continue;
				 break;
			}

			/* Get remaining data: data and checksum  */
			size = (pkg[1] < PKG_MAX_SIZE) ? (pkg[1] + 1) : (PKG_MAX_SIZE - 1);
			size = uart_rxfifo_wait(0, size);

			for (i=0; i < size; i++)
				pkg[i+2] = uart_getc_nowait(0);

			for (i=0; i < size + PKG_HEADER_SIZE; i++)
				printf("%x ", pkg[i]);

			printf("\r\n");

			xTaskNotify( xHandlingPkgTask, 0, eNoAction );

			/* Send to parser task */
			if (xQueueSend(rx_queue, (void *)pkg, 0) == pdFALSE) {
					printf("rx_queue queue overflow.\r\n");
			 }
		}
	}
}

static void  pkgParser_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint8_t pkg[PKG_MAX_SIZE];

    BaseType_t xResult;
    uint32_t ulNotifiedValue;

    while (1) {

    	xResult = xTaskNotifyWait( pdFALSE,          /* Don't clear bits on entry. */
								ULONG_MAX,        /* Clear all bits on exit. */
							   &ulNotifiedValue, /* Stores the notified value. */
							   portMAX_DELAY);


    	printf("pkgParser_task\r\n");


   	    while(xQueueReceive(rx_queue, (void *)pkg, 0) == pdTRUE){


   	    }


    }
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    uart_set_baud(1, 115200);
    
    /* Activate UART 1 PIN */
    gpio_set_iomux_function(2, IOMUX_GPIO2_FUNC_UART1_TXD);
    
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    vSemaphoreCreateBinary(wifi_alive);
    publish_queue = xQueueCreate(3, PUB_MSG_LEN);
    tx_queue = xQueueCreate(4, PKG_MAX_SIZE);
    rx_queue = xQueueCreate(4, PKG_MAX_SIZE);

    xTaskCreate(&wifi_task, "wifi_task",  256, NULL, 2, NULL);
    xTaskCreate(&uart_task, "uart_task", 256, NULL, 2, &xHandlingUartTask);

    xTaskCreate(&beat_task, "beat_task", 256, NULL, 3, NULL);
    xTaskCreate(&status_task, "status_task", 256, NULL, 3, NULL);

    xTaskCreate(&pkgParser_task, "pkgParser_task", 256, NULL, 3, &xHandlingPkgTask);

    xTaskCreate(&mqtt_task, "mqtt_task", 1024, NULL, 4, NULL);

}
