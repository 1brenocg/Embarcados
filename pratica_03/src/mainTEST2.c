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
	
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 *///https://sistemas.quixada.ufc.br/sippa/aluno_conferir_freq.jsp
int main(void){//https://sistemas.quixada.ufc.br/sippa/aluno_conferir_freq.jsp
	unsigned int port = GPIO1, pin12 = 12, pin16 = 16;
	
	flag=0x0;	
	/*-----------------------------------------------------------------------------
	 *  initialize GPIO module
	 *-----------------------------------------------------------------------------*/
	gpioInitModule(port);
	//gpioInitModule(port2); 

	/*-----------------------------------------------------------------------------
	 *  initialize pin of mudule
	 *-----------------------------------------------------------------------------*/
	gpioInitPin(port, pin12);
	gpioInitPin(port, pin16);

	/*-----------------------------------------------------------------------------
	 *  set pin direction 
	 *-----------------------------------------------------------------------------*/
	gpioSetDirection(port, pin12, OUTPUT);
	gpioSetDirection(port, pin16, INPUT);	
	
	int x;

	while(1){

//		gpioSetPinValue(port, pin12, 0);
//		delay();
//		gpioSetPinValue(port, pin12, 1);
//		delay();

		x = gpioGetPinValue(port, pin16);
		switch(x){
			case 0:
				gpioSetPinValue(port, pin12, 0);
				break;
			case 1:
				gpioSetPinValue(port, pin12, 1);
				break;
			default:
				break;
		}
	}
	delay();

	return(0);
} /* ----------  end of function main  ---------- */


/*FUNCTION*-------------------------------------------------------
*
* Function Name : Delay
* Comments      :
*END*-----------------------------------------------------------*/
static void delay(){
	volatile unsigned int ra;
	for(ra = 0; ra < TIME; ra ++);
}

