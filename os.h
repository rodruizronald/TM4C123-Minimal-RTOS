//*****************************************************************************
//
//  Prototypes for the OS kernel.
//  File: 		os.h
//  Version: 	1.0v
//  Author: 	Ronald Rodriguez Ruiz.
//  Date: 		May 18, 2019.	
//
//*****************************************************************************

#ifndef __OS_H__
#define __OS_H__
	
//*****************************************************************************
//
//	Prototypes for the API
//
//*****************************************************************************

extern void OS_start(void);
extern void OS_suspend(void);
extern void OS_sleep(uint32_t sleep_time);

extern int32_t OS_add_task(void (*p_task)(void), uint8_t priority);
extern int32_t OS_add_periodic_event(int32_t *p_sema, uint32_t period);


extern void OS_Semaphore_init(int32_t *p_sema, int32_t value);
extern void OS_Semaphore_pend(int32_t *p_sema);
extern void OS_Semaphore_post(int32_t *p_sema);

extern void OS_Fifo_init(void);
extern int8_t OS_Fifo_put(uint32_t data);
extern uint32_t OS_Fifo_get(void);

#endif	// __OS_H__
