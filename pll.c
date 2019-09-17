//*****************************************************************************
//  pll.c - Software functions for the Phase-Locked Loop (PLL)
//  Runs on LM4F120/TM4C123
//  Ronald Rodriguez Ruiz
//  August 16, 2018
//*****************************************************************************

#include "tm4c123gh6pm.h"
#include "pll.h"

//*****************************************************************************
//
//! @brief Initialize the Phase-Locked Loop (PLL).
//!
//! This function configures the system clock to 80 MHz from the PLL. With an
//! input crystal frecuency of 16MHz, the main oscillator, 400 MHz of PLL, and
//! a system clock diver of 4 (SYSDIV_4).
//!
//! @return None.
//
//*****************************************************************************
void PLL_init(void)
{
	//
	//	Configure the system to use RCC2 for advanced features
	//
	SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;

	//
	//	Bypass the PLL while initializing
	//
  SYSCTL_RCC_R  &= ~SYSCTL_RCC_USESYSDIV;
	SYSCTL_RCC2_R |=  SYSCTL_RCC2_BYPASS2;

	//
	//	Configure the crystal value (16MHz) and oscillator source (MOSC)
	//
	SYSCTL_RCC_R  &= ~SYSCTL_RCC_XTAL_M;
	SYSCTL_RCC_R  |=  SYSCTL_RCC_XTAL_16MHZ;
	SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;
	SYSCTL_RCC2_R |=  SYSCTL_RCC2_OSCSRC2_MO;

	//
	//	Clear the PLL lock interrupt
	//
	SYSCTL_MISC_R = SYSCTL_MISC_PLLLMIS;

	//
	//	Activate the PLL
	//
	SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;

	//
	//	Set the desired system divider, use 400 MHz PLL
	//
  	SYSCTL_RCC_R  |=   SYSCTL_RCC_USESYSDIV;
	SYSCTL_RCC2_R |=   SYSCTL_RCC2_DIV400;
	SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_SYSDIV2_M | SYSCTL_RCC2_SYSDIV2LSB);
	SYSCTL_RCC2_R |=  (SYSCTL_SYSDIV_4 << SYSCTL_RCC2_SYSDIV2LSB_S);
	
	//
	//	Wait until the PLL has locked
	//
	while((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0){};

	//
	//	Enable use of the PLL
	//
	SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
}
