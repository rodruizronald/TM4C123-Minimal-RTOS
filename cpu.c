//*****************************************************************************
//  pll.c - Software functions for the Phase-Locked Loop (PLL)
//  Runs on LM4F120/TM4C123
//  Ronald Rodriguez Ruiz
//  August 16, 2018
//*****************************************************************************

#include "cpu.h"

#if defined(ccs) //  Code Composer Studio Code

void CPU_disable_irq(void)
{
    //
    //  Disable interrupts.
    //
    __asm("  cpsid   i\n"
          "  bx      lr\n");
}

void CPU_enable_irq(void)
{
    //
    //  Enable interrupts.
    //
    __asm("  cpsie   i\n"
          "  bx      lr\n");
}

#endif

#if defined(rvmdk) || defined(__ARMCC_VERSION) //  Keil uVision Code

__asm void CPU_disable_irq(void)
{
    //
    //  Disable interrupts.
    //
    cpsid   i
    bx      lr
}

__asm void CPU_enable_irq(void)
{
    //
    //  Enable interrupts.
    //
    cpsie   i
    bx      lr
}

#endif
