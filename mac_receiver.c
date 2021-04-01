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
			//else data indication
			//check address
			
			//if address source not this station
				//if address destination not this station
					// send back to physic
				//else
					//compute checksum
					//if checksum ok
						//read= 1, ack = 1
						//send back to physic
						//send indication app
					//else
						//read= 1, ack = 0
						//send back to physic
					//end if
				//end if destination check	
			//else source this station
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
				//end if
					//else if token

		}

		
	}
}


