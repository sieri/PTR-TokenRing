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
	uint8_t* buffer[10];	//Array of queueMessage 
	uint8_t index = 0;	//Index for the array buffer
	uint8_t checksum = 0;	
	
	uint8_t token[TOKENSIZE]; //token storage
	
	for(;;)
	{
		//----------------------------------------------------------------------------
		//	QUEUE READ										
    //----------------------------------------------------------------------------

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
			if(index == 0)
			{
				//send token
				retCode = osMessageQueuePut(queue_phyS_id, &queueMsg, osPriorityNormal, 0);
				CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
			}
			else//else buffer not empty
			{
				/*QUEUE SEND*/ 	
				//put frame to queue
				queueMsg.type = TO_PHY;
				queueMsg.anyPtr = buffer[index-1];
				retCode = osMessageQueuePut(queue_phyS_id, &queueMsg, osPriorityNormal, 0);
				CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);


				index--;
				
				
				//TODO: fifo not lifo
			}
		}
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
		else if(queueMsg.type == DATA_IND)
		{
			/*store*/
			//store data in buffer
			buffer[index] = osMemoryPoolAlloc(memPool,osWaitForever);			
			*buffer[index] = (gTokenInterface.myAddress<<3) + (queueMsg.sapi&0x03);
			*(buffer[index]+1) = (queueMsg.addr<<3) + (queueMsg.sapi&0x03);
			*(buffer[index]+2) = 0;
			//calcul of the checksum
			int length = 0;
			checksum = 0;
			while(*(qPtr+length) != '\0')
			{
				*(buffer[index]+length+3) = *(qPtr+length);
				checksum += *(buffer[index]+length+3);
				length++;
			}

			*(buffer[index]+2) = length;
			
			//add bytes missing from checksum
			checksum += *buffer[index] + *(buffer[index]+1) + *(buffer[index]+2);
			*(buffer[index]+length+3) = checksum<<2;
			
			retCode = osMemoryPoolFree(memPool, qPtr);
			CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
			
			index++;	
			
		}
		//else if mac request
		else if(queueMsg.type == START)
		{
			//update ready list
			gTokenInterface.station_list[gTokenInterface.myAddress] = CHAT_SAPI+TIME_SAPI;
		}
		else if(queueMsg.type == STOP)
		{
			gTokenInterface.station_list[gTokenInterface.myAddress] = 0;
		}

		
	}
}


