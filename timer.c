//*****************************************************************************
//  timer.c - Software functions for Timers
//  Runs on LM4F120/TM4C123
//  Ronald Rodriguez Ruiz
//  November 16, 2018
//*****************************************************************************

#include <stdint.h>
#include "timer.h"
#include "tm4c123gh6pm.h"

//*****************************************************************************
//
//! @brief Initialize the Wide Timer5A.
//!
//! This function configures the Wide Timer5A to overflow based on the period 
//! passed and enables its IRQ with the given priority level.
//!
//! @param[in] period Overflow period.
//! @param[in] priority Interrupt priority level (from 0-7).
//!
//! @return None.
//
//*****************************************************************************
void Timer_WTimer5A_init(uint32_t period, uint8_t priority)
{
    //
    //  Enable wide timer5 clock gating
    //
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R5;
    //
    //  Delay to allow clock to stabilize
    // 
    while((SYSCTL_PRWTIMER_R & SYSCTL_PRWTIMER_R5) == 0){};
    //
    //  Disable wide timer5A during setup
    //
    WTIMER5_CTL_R &= ~TIMER_CTL_TAEN;
    //
    //  Set timer to 32-bit and count down mode
    // 
    WTIMER5_CFG_R = TIMER_CFG_16_BIT; 
    //
    //  Set timer to periodic mode
    //
    WTIMER5_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
    //
    //  Set reload value
    //
    WTIMER5_TAILR_R = period - 1;
    //
    //  No prescaler
    //
    WTIMER5_TAPR_R = 0;
    //
    //  Clear interrupt flag
    //            
    WTIMER5_ICR_R = TIMER_ICR_TATOCINT;
    // 
    //  Arm interrupt
    //
    WTIMER5_IMR_R |= TIMER_IMR_TATOIM;
    //
    //  Set interrupt priority
    // 
    NVIC_PRI26_R |= NVIC_PRI26_INTA_M  & (priority << NVIC_PRI26_INTA_S); 
    //
    //  Enable irq in NVIC
    // 
    NVIC_EN3_R |= NVIC_EN3_INT_M & (1 << 8);               
    //
    //  Enable wide timer5A 
    //
    WTIMER5_CTL_R |= TIMER_CTL_TAEN; 
}

//*****************************************************************************
//
//! @brief Initialize the System Timer (SysTick).
//!
//! This function configures the system timer to run under the core clock and
//! overflow based on the period passed.
//!
//! @param[in] period Overflow period of the SysTick.
//!
//! @return None.
//
//*****************************************************************************
void Timer_WTimer5A_clear_irq(void)
{
  WTIMER5_ICR_R = TIMER_ICR_TATOCINT;
}
