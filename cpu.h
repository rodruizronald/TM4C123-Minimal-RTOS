//*****************************************************************************
//  cpu.h - Prototypes and Register Definitions for the Phase-Locked Loop (PLL)
//  Runs on LM4F120/TM4C123
//  Ronald Rodriguez Ruiz
//  August 16, 2018
//*****************************************************************************

#ifndef __CPU_H__
#define __CPU_H__

//*****************************************************************************
//
//  Prototypes for the API
//
//*****************************************************************************

extern void CPU_disable_irq(void);
extern void CPU_enable_irq(void);

#endif  // __CPU_H__
