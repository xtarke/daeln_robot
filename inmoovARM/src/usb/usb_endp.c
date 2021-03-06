/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

#include <stdio.h>
#include "../data/Queue.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL 10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Pointer to a external packet queue */
extern void *USBQueue_ptr;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    : Data transfer from controller to host
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{
	//if(flag_TX_USB == 1)
	//{
//		UserToPMABufferCopy(packet_buffer, ENDP1_TXADDR, packet_size);
//		SetEPTxCount(ENDP1, packet_size);
//		SetEPTxValid(ENDP1);
//
//		flag_TX_USB = 0;
	//}
}


void send_data(uint8_t *packet_buffer, uint8_t packet_size)
{

	UserToPMABufferCopy(packet_buffer, ENDP1_TXADDR, packet_size);
	SetEPTxCount(ENDP1, packet_size);
	SetEPTxValid(ENDP1);
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    : Data transfer from host to controller
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
	uint8_t buffer[64], size, i;

	/* Get data from USB Endpoint */
	size = (uint8_t)(USB_SIL_Read(EP3_OUT, buffer));
	
	/* Enqueue all data */
	for (i=0; i < size; i++)
		if (USBQueue_ptr != NULL)
			Queue_enqueue(USBQueue_ptr, buffer[i]);

	/* Set rX valid Status */
	SetEPRxValid(ENDP3);
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    : CPU package - Start Of Frame.
				   Begin data transfer from controller to host.
				   After that, host will regularly call for more data ? Traduzido do russo!
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
  static uint32_t FrameCount = 0;

  if(bDeviceState == CONFIGURED)
  {
    if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
    {
      /* Reset the frame counter */
      FrameCount = 0;

      /* Check the data to be sent through IN pipe */
//      USB_Tx_State = 1;
			
	  /* Переход в Handle_USBAsynchXfer в файле hw_config.c */
      //Handle_USBAsynchXfer();
    }
  }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

