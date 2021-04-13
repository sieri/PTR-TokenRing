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



void sendToken()
{	
	struct queueMsg_t queueMsg;	//Queue message
	uint8_t *qPtr;							//Pointer for the queue
	osStatus_t retCode;					//For the return code
	
	
	//recreate new token
	queueMsg.type = TOKEN;
	
	qPtr = osMemoryPoolAlloc(memPool, osWaitForever);
	
	*qPtr = TOKEN_TAG;
	
	//clear token
	//update ready list
	for(uint32_t i = 0; i < 15 ;i++)
	{
		if(i != gTokenInterface.myAddress)
		{
			*(qPtr+i+1) = gTokenInterface.station_list[i];
		}
	}

	
	queueMsg.anyPtr = qPtr;


	queueMsg.type = TOKEN;
	retCode = osMessageQueuePut(queue_macS_id, &queueMsg, osPriorityNormal, 0);
	CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
}

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
						for(int i=0; i<qPtr[2]+3; i++)
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
					uint8_t status = qPtr[qPtr[2]+3] & 0x03;
					
					switch(status)
					{
						case 0x03:
							//read =1 acc =1
							//send token
						
							sendToken();
						
							break;
							
						case 0x02:
							//read = 1, acc=0
							//send message back
							//TODO
							break;
						
						case 0x00:
						case 0x01:
							//read = 0;
							//send mac error
							queueMsg.type = MAC_ERROR;
						
							retCode = osMessageQueuePut(queue_lcd_id, &queueMsg, osPriorityNormal, 0);
							CheckRetCode(retCode,__LINE__,__FILE__,CONTINUE);
							//send token
							sendToken();
					}

				}//end if
			}//else if token

		}

		
	}
}


