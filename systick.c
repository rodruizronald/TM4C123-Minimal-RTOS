//*****************************************************************************
//  systick.c - Software functions for the System Timer
//  Runs on LM4F120/TM4C123
//  Ronald Rodriguez Ruiz
//  August 16, 2018
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "systick.h"
#include "tm4c123gh6pm.h"

//*****************************************************************************
//
//! @brief Initialize the System Timer (SysTick).
//!
//! This function configures the system timer to run under the core clock with
//! and overflow time based on the period passed. The SysTick IRQ is enabled,
//! if specified, with the given priority level.
//!
//! @param[in] period Overflow period.
//! @param[in] irq_enabled Flag to enable the SysTick interrupt.
//! @param[in] priority Interrupt priority level (from 0-7).
//!
//! @return None.
//
//*****************************************************************************
void SysTick_init(uint32_t period, bool irq_enabled, uint8_t priority)
{
	//
	//	Disable SysTick during setup
	//
	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;

	//
	//	Set SysTick with core clock
	//
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC;

	//
	//	Set the reload value
	//
	NVIC_ST_RELOAD_R = period - 1;

	//
	//	Reset the counter
	//
	NVIC_ST_CURRENT_R = 0;

	//
	//	Enable SysTick clock and interrupt, if requested
	//
	if (irq_enabled)
	{
		//
		//	Interrupt priority 
		//
		NVIC_SYS_PRI3_R |= NVIC_SYS_PRI3_TICK_M & (priority << NVIC_SYS_PRI3_TICK_S);
		NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN;
	}
	else
	{
		NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
	}
}

//*****************************************************************************
//
//! @brief Wait the Systick count flag.
//!
//! This function loops until the SysTick has set the count flag, indicating
//! that the CURRENT_R has reached the max value. Duration of this loop
//! will rely on the RELOAD_R value (max value) set in SysTick_init().
//!
//! @return None.
//
//*****************************************************************************
void SysTick_wait(void)
{
	//
	//	Write to the current count to clear it
	//
	NVIC_ST_CURRENT_R = NVIC_ST_CURRENT_S;

	//
	//	Wait for the count flag
	//
	while((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0);
}

//*****************************************************************************
//
//! @brief Delays program execution.
//!
//! This function delays x number of milliseconds the program execution.
//!
//! @param[in] ms Delay time in milliseconds.
//!
//! @return None.
//
//*****************************************************************************
void SysTick_delay_ms(uint32_t ms)
{
	uint32_t i;

	//
	//	Since SysTick_wait() waits only 1 us, ms units must be multiply by 1000
	//
	ms *= 1000;

	//
	//	Loop x times (1 us each)
	//
	for(i = 0; i < ms; i++) SysTick_wait();
}

//*****************************************************************************
//
//! @brief Delays program execution.
//!
//! This function delays x number of microseconds the program execution.
//!
//! @param[in] us Delay time in microseconds.
//!
//! @return None.
//
//*****************************************************************************
void SysTick_delay_us(uint32_t us)
{
	unsigned long i;

	//
	//	Loop x times (1 us each)
	//
	for(i = 0; i < us; i++) SysTick_wait();
}

//*****************************************************************************
//
//! @brief Trigger the SysTick interrupt.
//!
//! This function sets the pending flag of the SysTick interrupt on the 
//! Interrupt Control Register to execute the SysTick_Handler.
//!
//! @return None.
//
//*****************************************************************************
void SysTick_set_pending(void)
{
	//
	//	Reset the counter
	//
	NVIC_ST_CURRENT_R = 0;
	
	//
	//	Change the SysTick exception state to pending
	//
	NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PENDSTSET;
}
