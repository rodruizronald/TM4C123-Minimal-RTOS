//*****************************************************************************
//  timer.h - Prototypes for Timers
//  Runs on LM4F120/TM4C123
//  Ronald Rodriguez Ruiz
//  November 16, 2018
//*****************************************************************************

#ifndef __TIMER_H__
#define __TIMER_H__

//*****************************************************************************
//
//  Prototypes for the API
//
//*****************************************************************************

void Timer_WTimer5A_init(uint32_t period, uint8_t priority);
void Timer_WTimer5A_clear_irq(void);

#endif  // __TIMER_H__
