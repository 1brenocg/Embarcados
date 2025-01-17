/**
 * \file   gpioLEDBlink.c
 *
 *  \brief  This application uses a GPIO pin to blink the LED.
 *
 *          Application Configurations:
 *
 *              Modules Used:
 *                  GPIO1
 *
 *              Configuration Parameters:
 *                  None
 *
 *          Application Use Case:
 *              1) The GPIO pin GPIO1[23] is used as an output pin.
 *              2) This pin is alternately driven HIGH and LOW. A finite delay
 *                 is given to retain the pin in its current state.
 *
 *          Running the example:
 *              On running the example, the LED on beaglebone would be seen 
 *              turning ON and OFF alternatively.
 *
 */

/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
*/
/* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "hw_control_AM335x.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
#define		PIN_HIGH   	1
#define        	PIN_LOW		0

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
static void Delay(unsigned int count);

/*****************************************************************************
**                INTERNAL FUNCTION DEFINITIONS
*****************************************************************************/
/*
** The main function. Application starts here.
*/
int main(){
	unsigned int modAddr=SOC_GPIO_1_REGS, mod=GPIO1, pin=21, dir=GPIO_DIR_OUTPUT ;
	
    	/* Enabling functional clocks for GPIO instance. */
    	GPIOModuleClkConfig(mod);
    
    	/* Selecting GPIO pin for use. */
    	GPIOPinMuxSetup(mod, pin);
    
	switch (mod) {
		case GPIO0:
			modAddr = SOC_GPIO_0_REGS;	
			break;
		case GPIO1:	
			modAddr = SOC_GPIO_1_REGS;	
			break;
		case GPIO2:	
			modAddr = SOC_GPIO_2_REGS;	
			break;
		case GPIO3:	
			modAddr = SOC_GPIO_3_REGS;	
			break;
		default:	
			break;
	}/* -----  end switch  ----- */

    	/* Setting the GPIO pin as an output pin. */
    	GPIODirModeSet(modAddr, pin, dir);

   	while(1){
        	/* Driving a logic HIGH on the GPIO pin. */
        	GPIOPinWrite(modAddr, pin, PIN_HIGH);

        	Delay(0x1FFFFF);

        	/* Driving a logic LOW on the GPIO pin. */
        	GPIOPinWrite(modAddr, pin, PIN_LOW);

        	Delay(0x1FFFFF);

    	}

	return(0);
} 

/*
** A function which is used to generate a delay.
*/
static void Delay(unsigned int count)
{
    while(count--);
}



/******************************* End of file *********************************/
