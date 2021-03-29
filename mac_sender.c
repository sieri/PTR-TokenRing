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
	
	for(;;)
	{
		//----------------------------------------------------------------------------
		//	QUEUE READ										
    //----------------------------------------------------------------------------

		//TODO: get data from queue
		
		//check token or data or request
		
		//if token
			
			//check buffer
			//if buffer empty
				//update ready list
				
				//update update State_st_i field
		
				//first time token if required
		
				//send token
		
				
		
			//else buffer not empty
			
				/*SEND*/ 
				//update ready list
				
				//update update State_st_i field
				
				//complete data frame
				
				//put frame to queue
				//buffer--
			//end if
			
		
		//end if token
		
		//else if data
			
			/*store*/
			//store data in buffer
			//buffer++
		
		//else if request
			//save request change
		
		
		//end 
		
	}
}


