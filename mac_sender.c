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
		if(qPtr[0] == TOKEN_TAG)
		{
			//check buffer
			//if buffer empty
				//update ready list
				
				//update update State_st_i field
		
				//first time token if required
				if(queueMsg.type == NEW_TOKEN)
				{
					//Create new token
				}
				//send token
		
			//else buffer not empty
			
				/*QUEUE SEND*/ 
				//update ready list
				
				//update update State_st_i field
				
				//complete data frame
				
				//put frame to queue
				//buffer--
			//end if
		}//end if token
		//else if data
		else if(queueMsg.type == FROM_PHY)
		{
			/*store*/
			//store data in buffer
			//buffer++
		}
		//else if request
		else if()
		{
			//save request change
		}//end 	
	}
}


