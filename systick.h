//*****************************************************************************
//  systick.h - Prototypes and Register Definitions for the System Timer
//  Runs on LM4F120/TM4C123
//  Ronald Rodriguez Ruiz
//  August 16, 2018
//*****************************************************************************

#ifndef __SYSTICK_H__
#define __SYSTICK_H__

//*****************************************************************************
//
//  Prototypes for the API
//
//*****************************************************************************

extern void SysTick_init(uint32_t period, bool irq_enabled, uint8_t priority);
extern void SysTick_wait(void);
extern void SysTick_delay_ms(uint32_t ms);
extern void SysTick_delay_us(uint32_t us);
extern void SysTick_set_pending(void);

#endif  // __SYSTICK_H__
