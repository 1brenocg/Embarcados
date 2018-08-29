/*
 * =====================================================================================
 *
 *       Filename:  finalProject.c
 *
 *    Description:  Despertador in BareMetal
 *
 *        Version:  1.0
 *        Created:  05/06/2017 09:36:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Breno Campos Gomes, breno.1803@hotmail.com
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

#include "uart_irda_cir.h"
#include "soc_AM335x.h"
#include "interrupt.h"
#include "board.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "consoleUtils.h"

#include "dmtimer.h"
#include "error.h"

#include "watchdog.h"
#include "hw_types.h"

/******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
static volatile unsigned int flagIsr = 0;

static volatile unsigned int flagIsr2 = 0;

static volatile unsigned int flagIsr3 = 0;

unsigned int digit1;
unsigned int digit2;
unsigned int digit3;
unsigned int digit4;

/*	A = 11 GPIO1_04 PINO 23
	B = 7  GPIO1_0	PINO 25
	C = 4  GPIO1_30 PINO 21
	D = 2  GPIO1_15 PINO 15
	E = 1  GPIO1_13 PINO 11
	F = 10 GPIO1_02 PINO 5
	G = 5  GPIO1_06 PINO 3

	TUDO DO P8

	D1 ESTA QUEIMADO
I
	D2 = GPIO2_1 PINO 18 
	D3 = GPIO2_3 PINO 8
	D4 = GPIO2_4 PINO 10

	TUDO DO P8

	BUT = GPIO1_31 PINO 20
	BUT2= GPIO1_29 PINO 26
	BUT3= GPIO1_30 PINO 21

	*/

unsigned int D2 = 1, D3 = 3, D4 = 4;

unsigned int a = 4, b = 0, c = 30, d = 15, e = 13, f = 2, g = 6;

unsigned int mod = 1, mod0 = 0, mod2 = 2;

unsigned int but = 31, but2 = 29, but3 = 30;

unsigned int n1 = 0, n2 = 0, n3 = 0;

unsigned min = 0, seg = 0;

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
#define TIMER_INITIAL_COUNT            (0xFF000000u)
#define TIMER_RLD_COUNT                (0xFFFFFF83u) //(0xFF000000u)

#define T_1MS_COUNT                     (0x5DC0u) 
#define OVERFLOW                        (0xFFFFFFFFu)
#define TIMER_1MS_COUNT                 (0x5DC0u) 

#define	PIN_HIGH   	1
#define PIN_LOW		0

/* Values denoting the Interrupt Line number to be used. */
#define GPIO_INTC_LINE_1                  (0x0)
#define GPIO_INTC_LINE_2                  (0x1)

/*
** Values used to enable/disable interrupt generation due to level
** detection on an input GPIO pin.
*/
#define GPIO_INTC_TYPE_NO_LEVEL           (0x01)
#define GPIO_INTC_TYPE_LEVEL_LOW          (0x04)
#define GPIO_INTC_TYPE_LEVEL_HIGH         (0x08)
#define GPIO_INTC_TYPE_BOTH_LEVEL         (0x0C)

/*
** Values used to enable/disable interrupt generation due to edge
** detection on an input GPIO pin.
*/
#define GPIO_INTC_TYPE_NO_EDGE            (0x80)
#define GPIO_INTC_TYPE_RISE_EDGE          (0x10)
#define GPIO_INTC_TYPE_FALL_EDGE          (0x20)
#define GPIO_INTC_TYPE_BOTH_EDGE          (0x30)

#define INITIAL_COUNT_VALUE          (0xFFFE0000u)
#define RELOAD_COUNT_VALUE           (0xFFFE0000u)

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/

static void getDigit(unsigned int);

static void DisplayNumber(unsigned int, unsigned int, unsigned int);

static void 		initSerial(void);
static void 		initLed(unsigned int, unsigned int, unsigned int);
static void 		initButton(unsigned int, unsigned int, unsigned int);
static void		gpioAintcConf(void);
static unsigned int  	getAddr(unsigned int);
static void 		gpioIsr(void);
static int 		gpioPinIntConf(unsigned int, unsigned int, unsigned int);
static void 		gpioPinIntEnable(unsigned int, unsigned int, unsigned int); 
static void 		gpioIntTypeSet(unsigned int, unsigned int, unsigned int);

static void DMTimerSetUp(void);
static void Delay(volatile unsigned int mSec);

/******************************************************************************
**              FUNCTION DEFINITIONS
******************************************************************************/



int main(){

/******************************************************************************
**                      DMTimer Fuctions - BEGIN
*******************************************************************************/
/* This function will enable clocks for the DMTimer2 instance */
    DMTimer2ModuleClkConfig();

    /* Initialize the UART console */
    ConsoleUtilsInit();

    /* Select the console type based on compile time check */
    ConsoleUtilsSetType(CONSOLE_UART);

    /* Perform the necessary configurations for DMTimer */
    DMTimerSetUp();
/******************************************************************************
**                      DMTimer Fuctions - END
*******************************************************************************/

	/*Base Andress of WatchDog = SOC_WDT_1_REGS */
	//WatchdogTimerDisable(SOC_WDT_1_REGS);

    initSerial();

    /* Enable IRQ in CPSR */
    IntMasterIRQEnable();

    /* configure gpio interrupt on the INTC */
   	gpioAintcConf();

    gpioPinIntConf(SOC_GPIO_1_REGS, GPIO_INTC_LINE_1, but);
    gpioPinIntConf(SOC_GPIO_1_REGS, GPIO_INTC_LINE_1, but2);
    gpioPinIntConf(SOC_GPIO_1_REGS, GPIO_INTC_LINE_1, but3);
    
    /* Enable interrupt generation on detection of a rising or a falling edge.*/
    gpioIntTypeSet(SOC_GPIO_1_REGS, but, GPIO_INTC_TYPE_RISE_EDGE);
    gpioIntTypeSet(SOC_GPIO_1_REGS, but2, GPIO_INTC_TYPE_RISE_EDGE);
    gpioIntTypeSet(SOC_GPIO_1_REGS, but3, GPIO_INTC_TYPE_RISE_EDGE);


    GPIOModuleClkConfig(GPIO1);
    GPIOModuleClkConfig(GPIO2);

    initLed(getAddr(mod2), mod2, D2);
    initLed(getAddr(mod2), mod2, D3);
    initLed(getAddr(mod2), mod2, D4);

	initLed(getAddr(mod), mod, a);
	initLed(getAddr(mod), mod, b);
	initLed(getAddr(mod), mod, c);
	initLed(getAddr(mod), mod, d);
	initLed(getAddr(mod), mod, e);
	initLed(getAddr(mod), mod, f);
	initLed(getAddr(mod), mod, g);

	initButton(getAddr(mod), mod, but);
	initButton(getAddr(mod), mod, but2);
	initButton(getAddr(mod), mod, but3);

	n1 = 1;
	n2 = 2;
	n3 = 3;
	while(TRUE){
		DisplayNumber(n1,n2,n3);
	}

/*	while(TRUE){
		if(flagIsr == 1){
			ConsoleUtilsPrintf("\n<<< Botao 1 Pressionado >>>\n")

			min++;

			flagIsr = 0;

			n1 = min;

			DisplayNumber(n1,n2,n3);
		}
		if(flagIsr2 == 1){
			ConsoleUtilsPrintf("\n<<< Botao 2 Pressionado >>>\n")

			seg++;

			n3 = seg;

			if(n3 == 9){
				n2++;

				n3 = 0;
			}

			flagIsr2 = 0;

			DisplayNumber(n1,n2,n3);
		}

		if(flagIsr3 == 1){

			ConsoleUtilsPrintf("\n<<< Botao 3 Pressionado >>>\n")

			unsigned int tempo;

			tempo = min * 60;

			tempo = tempo + seg;

			// tempo = tempo * 1000;

			while(tempo > 0){

				Delay(1000);
				tempo--;

				n3--;

				if(n3 == 0){
					n2--;
					n3 = 9;
					if (n2 == 0){
						n2 = 5;
						n1--;
						if(n1 == 0){
							n2 = 5;
							n3 = 9;
						}
					}
				}

				//DisplayNumber(n1,n2,n3);
			}
			flagIsr3 = 0;
		}

		DisplayNumber(n1,n2,n3);
	}*/

	ConsoleUtilsPrintf("#####  exit system  #####\n");
	
	return(0);
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : DisplayNumber
* Comments      : Mostra os numeros no display de 7 segmentos com 4 digitos
*END*-----------------------------------------------------------*/

static void DisplayNumber(unsigned int b1,unsigned int c1,unsigned int d1){
	
	//GPIO_UNKNOW1 = D1...
		//Digito 1

    /* if(TRUE){
			//desabilitando os digitos que nao seram usados
							
			GPIOPinWrite(getAddr(GPIO0), D1 , 1);
			GPIOPinWrite(getAddr(GPIO0), D2 , 0);
			GPIOPinWrite(getAddr(GPIO0), D3 , 0);
			GPIOPinWrite(getAddr(GPIO0), D4 , 0);

			switch(a1){
				case 0:

				    //desabilitando os digitos que nao seram usados
				
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;

				case 1:
					//desabilitando os digitos que nao seram usados
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;
				case 2:
					//desabilitando os digitos que nao seram usados
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 1);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 3:
					//desabilitando os digitos que nao seram usados
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 4:
					//desabilitando os digitos que nao seram usados
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 5:
				    //desabilitando os digitos que nao seram usados

					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 1);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 6:

				    //desabilitando os digitos que nao seram usados
				
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 1);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;

			    case 7:

				    //desabilitando os digitos que nao seram usados
				
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;
				case 8:
					//desabilitando os digitos que nao seram usados
				
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;	
				case 9:
					//desabilitando os digitos que nao seram usados
				
					GPIOPinWrite(getAddr(GPIO0), D1 , 0);
					GPIOPinWrite(getAddr(GPIO0), D2 , 1);
					GPIOPinWrite(getAddr(GPIO0), D3 , 1);
					GPIOPinWrite(getAddr(GPIO0), D4 , 1);

					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				default:
					break;
			}
		}*/

	//Digito 2

	if(TRUE){
			//desabilitando os digitos que nao seram usados
							
			// GPIOPinWrite(getAddr(GPIO2), D1 , 0);
			GPIOPinWrite(getAddr(GPIO2), D2 , 1);
			GPIOPinWrite(getAddr(GPIO2), D3 , 0);
			GPIOPinWrite(getAddr(GPIO2), D4 , 0);

			switch(b1){
				case 0:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;

				case 1:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;
				case 2:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 1);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 3:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 4:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 5:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 1);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 6:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 1);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
			    case 7:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;
				case 8:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;	
				case 9:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				default:
					break;
			}
	}

	//Digito 3

	if(TRUE){
			//desabilitando os digitos que nao seram usados
							
			// GPIOPinWrite(getAddr(GPIO2), D1 , 0);
			GPIOPinWrite(getAddr(GPIO2), D2 , 0);
			GPIOPinWrite(getAddr(GPIO2), D3 , 1);
			GPIOPinWrite(getAddr(GPIO2), D4 , 0);

			switch(c1){
				case 0:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;

				case 1:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;
				case 2:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 1);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 3:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 4:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 5:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 1);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 6:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 1);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;

			    case 7:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;
				case 8:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;	
				case 9:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				default:
					break;
			}
	}

	//Digito 4

	if(TRUE){
			//desabilitando os digitos que nao seram usados
			// GPIOPinWrite(getAddr(GPIO2), D1 , 0);
			GPIOPinWrite(getAddr(GPIO2), D2 , 0);
			GPIOPinWrite(getAddr(GPIO2), D3 , 0);
			GPIOPinWrite(getAddr(GPIO2), D4 , 1);

			switch(d1){
				case 0:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;

				case 1:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;
				case 2:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 1);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 3:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 4:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 5:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 1);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				case 6:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 1);
					GPIOPinWrite(getAddr(GPIO1), b , 1);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;

			    case 7:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 1);
					GPIOPinWrite(getAddr(GPIO1), g , 1);

					break;
				case 8:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 0);
					GPIOPinWrite(getAddr(GPIO1), e , 0);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;	
				case 9:
					//setando o numero no display

					GPIOPinWrite(getAddr(GPIO1), a , 0);
					GPIOPinWrite(getAddr(GPIO1), b , 0);
					GPIOPinWrite(getAddr(GPIO1), c , 0);
					GPIOPinWrite(getAddr(GPIO1), d , 1);
					GPIOPinWrite(getAddr(GPIO1), e , 1);
					GPIOPinWrite(getAddr(GPIO1), f , 0);
					GPIOPinWrite(getAddr(GPIO1), g , 0);

					break;
				default:
					break;
			}
	}
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : getDigit
* Comments      :Pega os digitos separados, e salva em 4 variaves globais
*END*-----------------------------------------------------------*/
/*static void getDigit (int number){
	int i;
	for(i = 9; i > 0; i--){
		if(i * 1000 <= number){
			digit1 = i;
			break;
		}
	}

	number -= i * 1000;

	for(i = 9; i > 0; i--){
		if (i * 100 <= number){
			digit2 = i;
			break;
		}
	}

	number -= i * 100;

	for(i = 9; i > 0; i--){
		if (i * 10 <= number){
			digit3 = i;
			break;
		}
	}

	number -= i * 10;

	digit4 = number;
}*/

/*
** Setup the timer for one-shot and compare mode.
*/

static void DMTimerSetUp(void){
    DMTimerReset(SOC_DMTIMER_2_REGS);

    /* Load the counter with the initial count value */
    //DMTimerCounterSet(SOC_DMTIMER_2_REGS, TIMER_INITIAL_COUNT);

    /* Load the load register with the reload count value */
    //DMTimerReloadSet(SOC_DMTIMER_2_REGS, TIMER_RLD_COUNT);
    /* Configure the DMTimer for Auto-reload and compare mode */
    DMTimerModeConfigure(SOC_DMTIMER_2_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : Delay
* Comments      :
*END*-----------------------------------------------------------*/
static void Delay(volatile unsigned int mSec){

	while(mSec != 0){

	     DMTimerCounterSet(SOC_DMTIMER_2_REGS, 0);

	     DMTimerEnable(SOC_DMTIMER_2_REGS);

	     while(DMTimerCounterGet(SOC_DMTIMER_2_REGS) < TIMER_1MS_COUNT);

	     DMTimerDisable(SOC_DMTIMER_2_REGS);

	     mSec--;
	}
}
/*FUNCTION*-------------------------------------------------------
*
* A function which is used to initialize UART.
*END*-----------------------------------------------------------*/
static unsigned  int getAddr(unsigned int module){
	unsigned int addr;

	switch (module) {
		case GPIO0:
			addr = SOC_GPIO_0_REGS;	
			break;
		case GPIO1:	
			addr = SOC_GPIO_1_REGS;	
			break;
		case GPIO2:	
			addr = SOC_GPIO_2_REGS;	
			break;
		case GPIO3:	
			addr = SOC_GPIO_3_REGS;	
			break;
		default:	
			break;
	}/* -----  end switch  ----- */

	return(addr);
}

/*FUNCTION*-------------------------------------------------------
*
* A function which is used to initialize UART.
*END*-----------------------------------------------------------*/
static void initSerial(){
	/* Initialize console for communication with the Host Machine */
    	ConsoleUtilsInit();

    	/* Select the console type based on compile time check */
    	ConsoleUtilsSetType(CONSOLE_UART);
}

/*FUNCTION*-------------------------------------------------------
*
* A function which is used to initialize GPIO like LED.
*END*-----------------------------------------------------------*/
static void initLed(unsigned int baseAdd, unsigned int module, unsigned int pin){
    
    	/* Selecting GPIO pin for use. */
    	GPIOPinMuxSetup(module, pin);
    
    	/* Setting the GPIO pin as an output pin. */
    	GPIODirModeSet(baseAdd, pin, GPIO_DIR_OUTPUT);
}

/*FUNCTION*-------------------------------------------------------
*
* A function which is used to initialize GPIO like BUTTON.
*END*-----------------------------------------------------------*/
static void initButton(unsigned int baseAdd, unsigned int module, unsigned int pin){
    
    	/* Selecting GPIO pin for use. */
    	GPIOPinMuxSetup(module, pin);
    
    	/* Setting the GPIO pin as an output pin. */
    	GPIODirModeSet(baseAdd, pin, GPIO_DIR_INPUT);
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : gpioAintcconfigure
* Comments      : Do the necessary gpio configurations on to AINTC.
*END*-----------------------------------------------------------*/
static void gpioAintcConf(void){

    /* Initialize the ARM interrupt control */
    IntAINTCInit();
 
    /* Registering gpioIsr */
    IntRegister(SYS_INT_GPIOINT1A, gpioIsr);
    
    /* Set the priority */
    IntPrioritySet(SYS_INT_GPIOINT1A, 0, AINTC_HOSTINT_ROUTE_IRQ);
    
    /* Enable the system interrupt */
    IntSystemEnable(SYS_INT_GPIOINT1A);
   
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : gpioIsr
* Comments      : DMTimer interrupt service routine. This will 
* send a character to serial.
*END*-----------------------------------------------------------*/    
static void gpioIsr(void) {
	if(GPIOPinRead(getAddr(mod), but)) flagIsr = 1;

	if(GPIOPinRead(getAddr(mod), but2)) flagIsr2 = 1;

	if(GPIOPinRead(getAddr(mod), but3)) flagIsr3 = 1;

    	/*	Clear wake interrupt	*/
	//HWREG(SOC_GPIO_1_REGS + 0x3C) = 0x1000;
	//HWREG(SOC_GPIO_1_REGS + 0x34) = 0x1000;

	HWREG(SOC_GPIO_1_REGS + 0x2C) = 0xFFFFFFFF;
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : gpioPinIntConfig
* Comments      :
*END*-----------------------------------------------------------*/
static int gpioPinIntConf(unsigned int baseAdd, unsigned int intLine,unsigned int pinNumber){

    	/* Setting interrupt GPIO pin. */
		gpioPinIntEnable(baseAdd, intLine,pinNumber);
 
    	return(0);
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : GPIOPinIntEnable
* Comments      : This API enables the configured interrupt event on a specified input
* GPIO pin to trigger an interrupt request.
*
* \param  baseAdd     The memory address of the GPIO instance being used
* \param  intLine     This specifies the interrupt request line on which the
*                     interrupt request due to the transitions on a specified
*                     pin be propagated
* \param  pinNumber   The number of the pin in the GPIO instance
*
* 'intLine' can take one of the following two values:
* - GPIO_INT_LINE_1 - interrupt request be propagated over interrupt line 1\n
* - GPIO_INT_LINE_2 - interrupt request be propagated over interrupt line 2\n
* 
* 'pinNumber' can take one of the following values:
* (0 <= pinNumber <= 31)\n
*
* \return None
*
*END*-----------------------------------------------------------*/
static void gpioPinIntEnable(unsigned int baseAdd, unsigned int intLine,unsigned int pinNumber){
    if(GPIO_INTC_LINE_1 == intLine){
        HWREG(baseAdd + GPIO_IRQSTATUS_SET(0)) = (1 << pinNumber);
    }else{
        HWREG(baseAdd + GPIO_IRQSTATUS_SET(1)) = (1 << pinNumber);
    }
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : gpioAintcconfigure
* Comments      : This API configures the event type for a specified 
* input GPIO pin. Whenever the selected event occurs on that GPIO pin 
* and if interrupt generation is enabled for that pin, the GPIO module 
* will send an interrupt to CPU.
*
* \param  baseAdd    The memory address of the GPIO instance being used
* \param  pinNumber  The number of the pin in the GPIO instance
* \param  eventType  This specifies the event type on whose detection,
*                    the GPIO module will send an interrupt to CPU,
*                    provided interrupt generation for that pin is enabled.
*
* 'pinNumber' can take one of the following values:
* (0 <= pinNumber <= 31)\n
*
* 'eventType' can take one of the following values:
* - GPIO_INT_TYPE_NO_LEVEL - no interrupt request on occurence of either a
*   logic LOW or a logic HIGH on the input GPIO pin\n
* - GPIO_INT_TYPE_LEVEL_LOW - interrupt request on occurence of a LOW level
*   (logic 0) on the input GPIO pin\n
* - GPIO_INT_TYPE_LEVEL_HIGH - interrupt request on occurence of a HIGH level
*   (logic 1) on the input GPIO pin\n
* - GPIO_INT_TYPE_BOTH_LEVEL - interrupt request on the occurence of both the
*   LOW level and HIGH level on the input GPIO pin\n
* - GPIO_INT_TYPE_NO_EDGE -  no interrupt request on either rising or
*   falling edges on the pin\n
* - GPIO_INT_TYPE_RISE_EDGE - interrupt request on occurence of a rising edge
*   on the input GPIO pin\n
* - GPIO_INT_TYPE_FALL_EDGE - interrupt request on occurence of a falling edge
*   on the input GPIO pin\n
* - GPIO_INT_TYPE_BOTH_EDGE - interrupt request on occurence of both a rising
*   and a falling edge on the pin\n
*
* \return  None
*
* \note  A typical use case of this API is explained below:
* 
*        If it is initially required that interrupt should be generated on a
*        LOW level only, then this API can be called with
*        'GPIO_INT_TYPE_LEVEL_LOW' as the parameter.
*        At a later point of time, if logic HIGH level only should be made as
*        the interrupt generating event, then this API has to be first called
*        with 'GPIO_INT_TYPE_NO_LEVEL' as the parameter and later with
*        'GPIO_INT_TYPE_LEVEL_HIGH' as the parameter. Doing this ensures that
*        logic LOW level trigger for interrupts is disabled.
*END*-----------------------------------------------------------*/
static void gpioIntTypeSet(unsigned int baseAdd,unsigned int pinNumber,unsigned int eventType){
    eventType &= 0xFF;

    switch(eventType){

        case GPIO_INT_TYPE_NO_LEVEL:

            /* Disabling logic LOW level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(0)) &= ~(1 << pinNumber);

            /* Disabling logic HIGH level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(1)) &= ~(1 << pinNumber);

        break;

        case GPIO_INT_TYPE_LEVEL_LOW:

            /* Enabling logic LOW level detect interrupt geenration. */
            HWREG(baseAdd + GPIO_LEVELDETECT(0)) |= (1 << pinNumber);

            /* Disabling logic HIGH level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(1)) &= ~(1 << pinNumber);

            /* Disabling rising edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_RISINGDETECT) &= ~(1 << pinNumber);

            /* Disabling falling edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_FALLINGDETECT) &= ~(1 << pinNumber);

        break;

        case GPIO_INT_TYPE_LEVEL_HIGH:

            /* Disabling logic LOW level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(0)) &= ~(1 << pinNumber);

            /* Enabling logic HIGH level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(1)) |= (1 << pinNumber);

            /* Disabling rising edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_RISINGDETECT) &= ~(1 << pinNumber);

            /* Disabling falling edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_FALLINGDETECT) &= ~(1 << pinNumber);
        
        break;

        case GPIO_INT_TYPE_BOTH_LEVEL:
            
            /* Enabling logic LOW level detect interrupt geenration. */
            HWREG(baseAdd + GPIO_LEVELDETECT(0)) |= (1 << pinNumber);

            /* Enabling logic HIGH level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(1)) |= (1 << pinNumber);

            /* Disabling rising edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_RISINGDETECT) &= ~(1 << pinNumber);

            /* Disabling falling edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_FALLINGDETECT) &= ~(1 << pinNumber);
            
        break;

        case GPIO_INT_TYPE_NO_EDGE:
            
            /* Disabling rising edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_RISINGDETECT) &= ~(1 << pinNumber);

            /* Disabling falling edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_FALLINGDETECT) &= ~(1 << pinNumber);

        break;

        case GPIO_INT_TYPE_RISE_EDGE:

            /* Enabling rising edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_RISINGDETECT) |= (1 << pinNumber);

            /* Disabling falling edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_FALLINGDETECT) &= ~(1 << pinNumber);

            /* Disabling logic LOW level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(0)) &= ~(1 << pinNumber);

            /* Disabling logic HIGH level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(1)) &= ~(1 << pinNumber);

        break;

        case GPIO_INT_TYPE_FALL_EDGE:

            /* Disabling rising edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_RISINGDETECT) &= ~(1 << pinNumber);

            /* Enabling falling edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_FALLINGDETECT) |= (1 << pinNumber);

            /* Disabling logic LOW level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(0)) &= ~(1 << pinNumber);

            /* Disabling logic HIGH level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(1)) &= ~(1 << pinNumber);

        break;

        case GPIO_INT_TYPE_BOTH_EDGE:

            /* Enabling rising edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_RISINGDETECT) |= (1 << pinNumber);

            /* Enabling falling edge detect interrupt generation. */
            HWREG(baseAdd + GPIO_FALLINGDETECT) |= (1 << pinNumber);

            /* Disabling logic LOW level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(0)) &= ~(1 << pinNumber);

            /* Disabling logic HIGH level detect interrupt generation. */
            HWREG(baseAdd + GPIO_LEVELDETECT(1)) &= ~(1 << pinNumber);

        break;

        default:
        break;
    }
}

/******************************* End of file *********************************/
