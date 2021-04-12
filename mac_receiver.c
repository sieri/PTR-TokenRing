//////////////////////////////////////////////////////////////////////////////////
/// \file mac_receiver.c
/// \brief MAC receiver thread
/// \author Pascal Sartoretti (sap at hevs dot ch)
/// \version 1.0 - original
/// \date  2018-02
//////////////////////////////////////////////////////////////////////////////////
#include "stm32f7xx_hal.h"

#include <stdio.h>
#include <string.h>
#include "main.h"


//////////////////////////////////////////////////////////////////////////////////
// THREAD MAC RECEIVER
//////////////////////////////////////////////////////////////////////////////////
void MacReceiver(void *argument)
{
	struct queueMsg_t queueMsg;	//Queue message
	uint8_t *msg;								//Pointer for the message
	uint8_t *qPtr;							//Pointer for the queue
	osStatus_t retCode;					//For the return code
	uint8_t checksum = 0;	
	
	for(;;)
	{
		//----------------------------------------------------------------------------
		//	QUEUE READ										
    //----------------------------------------------------------------------------

		//get data from queue
		retCode = osMessageQueueGet(queue_macR_id, &queueMsg, NULL, osWaitForever);
		CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);		//Verify the retCode
		qPtr = queueMsg.anyPtr;
		

		if(queueMsg.type == FROM_PHY)
		{
			if(qPtr[0] == TOKEN_TAG)
			{
				queueMsg.type = TOKEN;
				retCode = osMessageQueuePut(queue_macS_id, &queueMsg, osPriorityNormal, 0);
				CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
			}
			else//else data indication
			{
			//check address
			
			//if address source not this station
				if(qPtr[0]>>3 != gTokenInterface.myAddress)
				{				
					//if address destination not this station
					if(qPtr[1]>>3 != gTokenInterface.myAddress)
					{
						// send back to physic
						queueMsg.type = TO_PHY;
						retCode = osMessageQueuePut(queue_phyS_id, &queueMsg, osPriorityNormal, 0);
						CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
					}
					else
					{
						//calcul of the checksum
						checksum = 0;
						for(int i=3; i<qPtr[2]; i++)
						{
							checksum += qPtr[i];
						}
						//if checksum ok
						if((checksum&0x3F) == qPtr[qPtr[2]+3]>>2)
						{
							//read= 1, ack = 1
							qPtr[qPtr[2]+3] += 0x03;
							//send back to physic
							queueMsg.type = TO_PHY;
							retCode = osMessageQueuePut(queue_phyS_id, &queueMsg, osPriorityNormal, 0);
							CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
							//send indication app
							queueMsg.type = DATA_IND;
							msg = osMemoryPoolAlloc(memPool,osWaitForever);
							memcpy(msg, &qPtr[3], qPtr[2]);
							msg[qPtr[2]] = '\0';
							queueMsg.anyPtr = msg;
							queueMsg.addr = (qPtr[0]>>3);
							queueMsg.sapi = qPtr[0]&0x7;
							if(queueMsg.sapi == CHAT_SAPI)
							{
								retCode = osMessageQueuePut(queue_chatR_id, &queueMsg, osPriorityNormal, 0);
								CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
							}
							else if(queueMsg.sapi == TIME_SAPI)
							{
								retCode = osMessageQueuePut(queue_timeR_id, &queueMsg, osPriorityNormal, 0);
								CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
							}
						}
						else
						{
							//read= 1, ack = 0
							qPtr[qPtr[2]+3] += 0x02;
							//send back to physic
							queueMsg.type = TO_PHY;
							retCode = osMessageQueuePut(queue_phyS_id, &queueMsg, osPriorityNormal, 0);
							CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
						}
					}
				}
				else//else source this station
				{
				//check read bit
				//if read = 1
					//if ack = 1
						//send token
					//else 
						//read= 0, ack = 0
						//send back to physic
					//end if
				//else read 0
					//send mac error
					//send token
				}//end if
			}//else if token

		}

		
	}
}


