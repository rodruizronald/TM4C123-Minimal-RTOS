//*****************************************************************************
//  pll.h - Prototypes and Register Definitions for the Phase-Locked Loop (PLL)
//  Runs on LM4F120/TM4C123
//  Ronald Rodriguez Ruiz
//  August 16, 2018
//*****************************************************************************

#ifndef __PLL_H__
#define __PLL_H__

//*****************************************************************************
//
//  The following are defines for the bit fields in the SYSCTL_RCC2 register
//
//*****************************************************************************

#define SYSCTL_RCC2_SYSDIV2LSB_S  22

//*****************************************************************************
//
//  The following are defines for the SYSDIV bit fields in the RCC/RCC2 register
//
//*****************************************************************************

#define SYSCTL_SYSDIV_1           1  // Processor clock is osc/pll /1
#define SYSCTL_SYSDIV_2           2  // Processor clock is osc/pll /2
#define SYSCTL_SYSDIV_3           3  // Processor clock is osc/pll /3
#define SYSCTL_SYSDIV_4           4  // Processor clock is osc/pll /4
#define SYSCTL_SYSDIV_5           5  // Processor clock is osc/pll /5
#define SYSCTL_SYSDIV_6           6  // Processor clock is osc/pll /6
#define SYSCTL_SYSDIV_7           7  // Processor clock is osc/pll /7
#define SYSCTL_SYSDIV_8           8  // Processor clock is osc/pll /8
#define SYSCTL_SYSDIV_9           9  // Processor clock is osc/pll /9
#define SYSCTL_SYSDIV_10          10  // Processor clock is osc/pll /10
#define SYSCTL_SYSDIV_11          11  // Processor clock is osc/pll /11
#define SYSCTL_SYSDIV_12          12  // Processor clock is osc/pll /12
#define SYSCTL_SYSDIV_13          13  // Processor clock is osc/pll /13
#define SYSCTL_SYSDIV_14          14  // Processor clock is osc/pll /14
#define SYSCTL_SYSDIV_15          15  // Processor clock is osc/pll /15

//*****************************************************************************
//
//  Prototypes for the API
//
//*****************************************************************************

void PLL_init(void);

#endif  // __PLL_H__
