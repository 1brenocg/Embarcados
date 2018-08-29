/**
 * \file   gpio.c
 *
 * \brief  This file contains functions which performs the platform specific
 *         configurations of GPIO.
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
#include "hw_cm_wkup.h"
#include "beaglebone.h"
#include "hw_cm_per.h"
#include "hw_types.h"

/**
 * \brief  This function does the pin multiplexing for any GPIO Pin.
 *
 * \param  offsetAddr   This is the offset address of the Pad Control Register
 *                      corresponding to the GPIO pin. These addresses are
 *                      offsets with respect to the base address of the
 *                      Control Module.
 * \param  padConfValue This is the value to be written to the Pad Control
 *                      register whose offset address is given by 'offsetAddr'.
 *
 * The 'offsetAddr' and 'padConfValue' can be obtained from macros defined
 * in the file 'include/armv7a/am335x/pin_mux.h'.\n
 *
 * \return  None.
 */
void GPIOPinMuxSetup(gpioMod module, ucPinNumber pin){
	unsigned int offsetAddr=CONTROL_CONF_GPMC_A(5),  mode=0, padConfValue;
	
	padConfValue = CONTROL_CONF_MUXMODE(7);
	
	switch (module) {
		case GPIO0:
			switch (pin) {
				  case 0:
					    offsetAddr = CONTROL_CONF_MDIO_DATA;
				      break;
				  case 1:	
					    offsetAddr = CONTROL_CONF_MDIO_CLK;	
				      break;
				  default:	
				      break;
			}/* -----  end switch  ----- */
			break;
		case GPIO1:	
			switch (pin) {
				case 0 ... 15:
					  mode = pin;
					  offsetAddr = CONTROL_CONF_GPMC_AD(mode);
				    break;
				case 16 ... 27:
					  mode = pin - 16;	
					  offsetAddr = CONTROL_CONF_GPMC_A(mode);
				    break;
				case 28:
					  offsetAddr = CONTROL_CONF_GPMC_BE1N;
				    break;
				case 29 ... 31:
					  mode = pin - 29;
					  offsetAddr = CONTROL_CONF_GPMC_CSN(mode);
				    break;
				default:	
				    break;
			}/* -----  end switch  ----- */
			break;
		case GPIO2:
      switch (pin) {
        case 1:
            offsetAddr = CONTROL_CONF_GPMC_CLK;
            break;
        case 3:
            offsetAddr = CONTROL_CONF_GPMC_OEN_REN;
            break;
        case 4:
            offsetAddr = CONTROL_CONF_GPMC_WEN;
            break;
        default:
            break;
      }
      /* -----  end switch  ----- */
      break;
		case GPIO3:	
			break;
		default:	
			break;
	}/* -----  end switch  ----- */

    	HWREG(SOC_CONTROL_REGS + offsetAddr) |= (padConfValue);
}

/*
** This function enables the L3 and L4_WKUP interface clocks.
** This also enables the functional clock for GPIO0 instance.
*/
void GPIOModuleClkConfig(unsigned int module){
	switch (module) {
		case GPIO0:	
			/* Writing to MODULEMODE field of CM_WKUP_GPIO0_CLKCTRL register. */
    			HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) |=
        			CM_WKUP_GPIO0_CLKCTRL_MODULEMODE_ENABLE;

    			/* Waiting for MODULEMODE field to reflect the written value. */
    			while(CM_WKUP_GPIO0_CLKCTRL_MODULEMODE_ENABLE !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) &
           			CM_WKUP_GPIO0_CLKCTRL_MODULEMODE));

    			/*
    			** Writing to OPTFCLKEN_GPIO0_GDBCLK field of CM_WKUP_GPIO0_CLKCTRL
    			** register.
    			*/
    			HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) |=
        			CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK;

    			/* Waiting for OPTFCLKEN_GPIO0_GDBCLK field to reflect the written value. */
    			while(CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) &
           			CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK));

    			/* Verifying if the other bits are set to required settings. */

    			/*
    			** Waiting for IDLEST field in CM_WKUP_CONTROL_CLKCTRL register to attain
    			** desired value.
    			*/
  	 	 	while((CM_WKUP_CONTROL_CLKCTRL_IDLEST_FUNC <<
           			CM_WKUP_CONTROL_CLKCTRL_IDLEST_SHIFT) !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CONTROL_CLKCTRL) &
           			CM_WKUP_CONTROL_CLKCTRL_IDLEST));

    			/*
    			** Waiting for CLKACTIVITY_L3_AON_GCLK field in CM_L3_AON_CLKSTCTRL
    			** register to attain desired value.
    			*/
    			while(CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKACTIVITY_L3_AON_GCLK !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_L3_AON_CLKSTCTRL) &
           			CM_WKUP_CM_L3_AON_CLKSTCTRL_CLKACTIVITY_L3_AON_GCLK));

    			/*
    			** Waiting for IDLEST field in CM_WKUP_L4WKUP_CLKCTRL register to attain
    			** desired value.
    			*/
    			while((CM_WKUP_L4WKUP_CLKCTRL_IDLEST_FUNC <<
           			CM_WKUP_L4WKUP_CLKCTRL_IDLEST_SHIFT) !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_L4WKUP_CLKCTRL) &
           			CM_WKUP_L4WKUP_CLKCTRL_IDLEST));

    			/*
    			** Waiting for CLKACTIVITY_L4_WKUP_GCLK field in CM_WKUP_CLKSTCTRL register
    			** to attain desired value.
    			*/
    			while(CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL) &
           			CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK));

    			/*
    			** Waiting for CLKACTIVITY_L4_WKUP_AON_GCLK field in CM_L4_WKUP_AON_CLKSTCTRL
    			** register to attain desired value.
    			*/
    			while(CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL_CLKACTIVITY_L4_WKUP_AON_GCLK !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL) &
           			CM_WKUP_CM_L4_WKUP_AON_CLKSTCTRL_CLKACTIVITY_L4_WKUP_AON_GCLK));


    			/* Writing to IDLEST field in CM_WKUP_GPIO0_CLKCTRL register. */
    			while((CM_WKUP_GPIO0_CLKCTRL_IDLEST_FUNC <<
           			CM_WKUP_GPIO0_CLKCTRL_IDLEST_SHIFT) !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) &
           			CM_WKUP_GPIO0_CLKCTRL_IDLEST));

    			/*
    			** Waiting for CLKACTIVITY_GPIO0_GDBCLK field in CM_WKUP_GPIO0_CLKCTRL
    			** register to attain desired value.
    			*/
    			while(CM_WKUP_CLKSTCTRL_CLKACTIVITY_GPIO0_GDBCLK !=
          			(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL) &
           			CM_WKUP_CLKSTCTRL_CLKACTIVITY_GPIO0_GDBCLK));

			break;

		case GPIO1:	    
			/* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
    			HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) |=
          			CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;

    			/* Waiting for MODULEMODE field to reflect the written value. */
    			while(CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE !=
          			(HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
           			CM_PER_GPIO1_CLKCTRL_MODULEMODE));
   	 		/*
    			** Writing to OPTFCLKEN_GPIO_1_GDBCLK bit in CM_PER_GPIO1_CLKCTRL
   	 		** register.
    			*/
    			HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) |=
          			CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK;

    			/*
    			** Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired
    			** value.
    			*/
    			while(CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK !=
         	 		(HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
           			CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK));

    			/*
    			** Waiting for IDLEST field in CM_PER_GPIO1_CLKCTRL register to attain the
    			** desired value.
    			*/
    			while((CM_PER_GPIO1_CLKCTRL_IDLEST_FUNC <<
           			CM_PER_GPIO1_CLKCTRL_IDLEST_SHIFT) !=
           			(HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
            			CM_PER_GPIO1_CLKCTRL_IDLEST));

    			/*
    			** Waiting for CLKACTIVITY_GPIO_1_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL
    			** register to attain desired value.
    			*/
    			while(CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK !=
          			(HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           			CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK));

			break;
		
		case GPIO2:	
          /* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
          HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
                CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE;

          /* Waiting for MODULEMODE field to reflect the written value. */
          while(CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE !=
                (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
                CM_PER_GPIO2_CLKCTRL_MODULEMODE));
        /*
          ** Writing to OPTFCLKEN_GPIO_1_GDBCLK bit in CM_PER_GPIO1_CLKCTRL
        ** register.
          */
          HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
                CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK;

          /*
          ** Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired
          ** value.
          */
          while(CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK !=
              (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
                CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK));

          /*
          ** Waiting for IDLEST field in CM_PER_GPIO1_CLKCTRL register to attain the
          ** desired value.
          */
          while((CM_PER_GPIO2_CLKCTRL_IDLEST_FUNC <<
                CM_PER_GPIO2_CLKCTRL_IDLEST_SHIFT) !=
                (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
                  CM_PER_GPIO2_CLKCTRL_IDLEST));

          /*
          ** Waiting for CLKACTIVITY_GPIO_1_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL
          ** register to attain desired value.
          */
          while(CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK !=
                (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
                CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK));
			break;


		case GPIO3:	
			break;

		default:	
			break;
	}/* -----  end switch  ----- */
}


/****************************** End of file *********************************/
