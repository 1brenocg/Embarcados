/*s
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/02/2017 20:05:55
 *       Revision:  none
 *       Compiler:  arm-none-eabi-gcc
 *
 *         Author:  Francisco Helder (FHC), helderhdw@gmail.com
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

#include	"gpio.h"

int flag;

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
#define TIME		2000000
#define TOGGLE          (0x01u)

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
static void delay();
static void toggle(gpioPort port, ucPinNumber pin, int value);
	
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main(void){
	unsigned int port=GPIO1, pin=14, pin2=12, pin3=13;
	
	flag=0x0;	
	/*-----------------------------------------------------------------------------
	 *  initialize GPIO module
	 *-----------------------------------------------------------------------------*/
	gpioInitModule(port);
	//gpioInitModule(port2); 

	/*-----------------------------------------------------------------------------
	 *  initialize pin of mudule
	 *-----------------------------------------------------------------------------*/
	gpioInitPin(port, pin);
	gpioInitPin(port, pin2);
	gpioInitPin(port, pin3);

	/*-----------------------------------------------------------------------------
	 *  set pin direction 
	 *-----------------------------------------------------------------------------*/
	gpioSetDirection(port, pin, INPUT);
	gpioSetDirection(port, pin2, OUTPUT);
	gpioSetDirection(port, pin3, OUTPUT);

	int value;

	while(TRUE){
		value = gpioGetPinValue(port, pin);

		switch(value){
			case LOW:
					gpioSetPinValue(port, pin2, LOW);
					gpioSetPinValue(port, pin3, HIGH);
					break;
			case HIGH:
					gpioSetPinValue(port, pin3, LOW);
					gpioSetPinValue(port, pin2, HIGH);
					break;
			default:
					break;
		}
		
	}

	return(0);
} /* ----------  end of function main  ---------- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  toggle
 *  Description:  
 * =====================================================================================
 */
void toggle(gpioPort port, ucPinNumber pin, int value){
	//flag^=TOGGLE;

	if(value)
		gpioSetPinValue(port, pin, HIGH);
	else
		gpioSetPinValue(port, pin, LOW);	
}		/* -----  end of function GPIO_toggle  ----- */


/*FUNCTION*-------------------------------------------------------
*
* Function Name : Delay
* Comments      :
*END*-----------------------------------------------------------*/
static void delay(){
	volatile unsigned int ra;
	for(ra = 0; ra < TIME; ra ++);
}

