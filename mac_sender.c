//////////////////////////////////////////////////////////////////////////////////
/// \file mac_sender.c
/// \brief MAC sender thread
/// \author Pascal Sartoretti (pascal dot sartoretti at hevs dot ch)
/// \version 1.0 - original
/// \date  2018-02
//////////////////////////////////////////////////////////////////////////////////
#include "stm32f7xx_hal.h"

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "ext_led.h"


//////////////////////////////////////////////////////////////////////////////////
// THREAD MAC RECEIVER
//////////////////////////////////////////////////////////////////////////////////
void MacSender(void *argument)
{
	struct queueMsg_t queueMsg;	//Queue message
	uint8_t *qPtr;							//Pointer for the queue
	osStatus_t retCode;					//For the return code
	
	uint8_t token[TOKENSIZE]; //token storage
	
	for(;;)
	{
		//----------------------------------------------------------------------------
		//	QUEUE READ										
    //----------------------------------------------------------------------------

		//TODO: get data from queue
		retCode = osMessageQueueGet(queue_macS_id, &queueMsg, NULL, osWaitForever);
		CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);		//Verify the retCode
		qPtr = queueMsg.anyPtr;

		
		//check token or data or request
		
		//if token
		
		if(queueMsg.type == TOKEN)
		{
			//check buffer
			//update ready list
			for(uint32_t i = 0; i < 15 ;i++)
			{
				if(i != gTokenInterface.myAddress)
				{
					gTokenInterface.station_list[i] = *(qPtr+i+1);
				}
				else
				{
					//update update State_st_i field
					*(qPtr+i+1) = gTokenInterface.station_list[i];
				}

			}
			
			
			//if buffer empty

				//send token
				retCode = osMessageQueuePut(queue_phyS_id, &queueMsg, osPriorityNormal, 0);
				CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
		
			//else buffer not empty
			
				/*QUEUE SEND*/ 

				
				//complete data frame
				
				//put frame to queue
				//buffer--
			//end if
		}//end if token
		//first time token if required
		else if(queueMsg.type == NEW_TOKEN)
		{
			//Create new token
			queueMsg.type = TOKEN;
			
			token[0] = TOKEN_TAG;
			
			//clear token
			memset(token+1, 0, TOKENSIZE-1);
			
			queueMsg.anyPtr = token;
			
			//send token
			retCode = osMessageQueuePut(queue_phyS_id, &queueMsg, osPriorityNormal, 0);
			CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
		}
		//else if data
		else if(queueMsg.type == FROM_PHY)
		{
			/*store*/
			//store data in buffer
			//buffer++
		}
		//else if mac request
		else if(queueMsg.type == START)
		{
			//update ready list
			
		}
		else if(queueMsg.type == STOP)
		{
			//update ready list
		}
		else if(queueMsg.type == TOKEN_LIST)
		{
			//push ready lists
		}
		
	}
}


