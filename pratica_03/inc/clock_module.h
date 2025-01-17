#ifndef __CLOCK_MODULE_H
#define __CLOCK_MODULE_H

/*
 * =====================================================================================
 *
 *       Filename:  clock_module.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/02/2017 19:44:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Francisco Helder (FHC), helderhdw@gmail.com
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  ENUM
 *-----------------------------------------------------------------------------*/
typedef enum{
   CKM_PER_L4LS_CLKSTCTRL      = 0x000,
   CKM_PER_L3S_CLKSTCTRL       = 0x004,
   CKM_PER_L4FW_CLKSTCTRL      = 0x008,
   CKM_PER_L3_CLKSTCTRL        = 0x00C,
   CKM_PER_CPGMAC0_CLKCTRL     = 0x014,
   CKM_PER_LCDC_CLKCTRL        = 0x018,
   CKM_PER_USB0_CLKCTRL        = 0x01C,
   CKM_PER_TPTC0_CLKCTRL       = 0x024,
   CKM_PER_EMIF_CLKCTRL        = 0x028,
   CKM_PER_OCMCRAM_CLKCTRL     = 0x02C,
   CKM_PER_GPMC_CLKCTRL        = 0x030,
   CKM_PER_MCASP0_CLKCTRL      = 0x034,
   CKM_PER_UART5_CLKCTRL       = 0x038,
   CKM_PER_MMC0_CLKCTRL        = 0x03C,
   CKM_PER_ELM_CLKCTRL         = 0x040,
   CKM_PER_I2C2_CLKCTRL        = 0x044,
   CKM_PER_I2C1_CLKCTRL        = 0x048,
   CKM_PER_SPI0_CLKCTRL        = 0x04C,
   CKM_PER_SPI1_CLKCTRL        = 0x050,
   CKM_PER_L4LS_CLKCTRL        = 0x060,
   CKM_PER_L4FW_CLKCTRL        = 0x064,
   CKM_PER_MCASP1_CLKCTRL      = 0x068,
   CKM_PER_UART1_CLKCTRL       = 0x06C,
   CKM_PER_UART2_CLKCTRL       = 0x070,
   CKM_PER_UART3_CLKCTRL       = 0x074,
   CKM_PER_UART4_CLKCTRL       = 0x078,
   CKM_PER_TIMER7_CLKCTRL      = 0x07C,
   CKM_PER_TIMER2_CLKCTRL      = 0x080,
   CKM_PER_TIMER3_CLKCTRL      = 0x084,
   CKM_PER_TIMER4_CLKCTRL      = 0x088,
   CKM_PER_GPIO1_CLKCTRL       = 0x0AC,
   CKM_PER_GPIO2_CLKCTRL       = 0x0B0,
   CKM_PER_GPIO3_CLKCTRL       = 0x0B4,
   CKM_PER_TPCC_CLKCTRL        = 0x0BC,
   CKM_PER_DCAN0_CLKCTRL       = 0x0C0,
   CKM_PER_DCAN1_CLKCTRL       = 0x0C4,
   CKM_PER_EPWMSS1_CLKCTRL     = 0x0CC,
   CKM_PER_EMIF_FW_CLKCTRL     = 0x0D0,
   CKM_PER_EPWMSS0_CLKCTRL     = 0x0D4,
   CKM_PER_EPWMSS2_CLKCTRL     = 0x0D8,
   CKM_PER_L3_INSTR_CLKCTRL    = 0x0DC,
   CKM_PER_L3_CLKCTRL          = 0x0E0,
   CKM_PER_IEEE5000_CLKCTRL    = 0x0E4,
   CKM_PER_PRU_ICSS_CLKCTRL    = 0x0E8,
   CKM_PER_TIMER5_CLKCTRL      = 0x0EC,
   CKM_PER_TIMER6_CLKCTRL      = 0x0F0,
   CKM_PER_MMC1_CLKCTRL        = 0x0F4,
   CKM_PER_MMC2_CLKCTRL        = 0x0F8,
   CKM_PER_TPTC1_CLKCTRL       = 0x0FC,
   CKM_PER_TPTC2_CLKCTRL       = 0x100,
   CKM_PER_SPINLOCK_CLKCTRL    = 0x10C,
   CKM_PER_MAILBOX0_CLKCTRL    = 0x110,
   CKM_PER_L4HS_CLKSTCTRL      = 0x11C,
   CKM_PER_L4HS_CLKCTRL        = 0x120,
   CKM_PER_OCPWP_L3_CLKSTCTRL  = 0x12C,
   CKM_PER_OCPWP_CLKCTRL       = 0x130,
   CKM_PER_PRU_ICSS_CLKSTCTRL  = 0x140,
   CKM_PER_CPSW_CLKSTCTRL      = 0x144,
   CKM_PER_LCDC_CLKSTCTRL      = 0x148,
   CKM_PER_CLKDIV32K_CLKCTRL   = 0x14C,
   CKM_PER_CLK_24MHZ_CLKSTCTRL = 0x150,
}CKM_MODULE_REG;


/*-----------------------------------------------------------------------------
 *  Function prototype
 *-----------------------------------------------------------------------------*/
void ckmSetCLKModuleRegister(CKM_MODULE_REG module, unsigned int value);
unsigned int ckmGetCLKModuleRegister(CKM_MODULE_REG module);

#endif /* __CLOCK_MODULE__H */

