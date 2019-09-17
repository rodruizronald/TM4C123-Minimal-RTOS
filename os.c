//*****************************************************************************
//
//  API and private functions for the OS kernel.
//  File: 		os.c
//  Version: 	1.0v
//  Author: 	Ronald Rodriguez Ruiz.
//  Date: 		May 18, 2019.		
//
//*****************************************************************************
//*****************************************************************************
//
//  The following are header files for the C standard library.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
//  This is the application header file.
//
//*****************************************************************************

#include "os.h"

//*****************************************************************************
//
//  The following are header files for the TM4C123G driver library.
//
//*****************************************************************************

#include "cpu.h"
#include "pll.h"
#include "timer.h"
#include "systick.h"

//*****************************************************************************
//
//  The following are defines for the operating frequencies (Hz) of the OS.
//
//*****************************************************************************

#define CPU_CLOCK_FREQ		80000000
#define EVENT_FREQ 			1000   		// Frequency at which the real-time 
										// events are executed
#define TASK_FREQ 			1000		// Frequency at which tasks are 
										// switched 

//*****************************************************************************
//
//  The following are defines for the TCB and FIFO static memory allocation.
//
//*****************************************************************************

#define NUM_TASKS  	  		8        	// max number of task
#define NUM_EVENTS  	  	2        	// max number of events
#define STACK_SIZE   		100			// number of 32-bit words per task
#define FIFO_SIZE 			10    		// max number of entries in the FIFO

//*****************************************************************************
//
//  This data structure defines the Task Control Block (TCB).
//
//*****************************************************************************

struct tcb
{
	uint32_t *sp;			// pointer to stack
	int32_t *blocked;    	// nonzero if blocked on this semaphore
	uint32_t sleep;   		// nonzero if this task is sleep
	uint8_t priority;    	// 0 is highest, 254 is lowest
	struct tcb *next;    	// linked-list pointer
};

//*****************************************************************************
//
//  This data structure defines the Event Control Block (ECB).
//
//*****************************************************************************

struct ecb
{
	int32_t *semaphore;		 
	uint32_t period;		
};

//*****************************************************************************
//
//  The following are global definitios for the TCBs and ECBs.
//
//*****************************************************************************

static struct tcb g_tcbs[NUM_TASKS];			// one TCB per task 
struct tcb *gp_running_task;					// pointer to the running task
static uint32_t g_stacks[NUM_TASKS][STACK_SIZE];// a hundred elments per task
static struct ecb g_ecbs[NUM_EVENTS];			// one ECB per event
static uint8_t g_event_cnt = 0;					// number of events added

//*****************************************************************************
//
//	The following are global definitios for the FIFO.
//
//*****************************************************************************

static uint32_t g_fifo[FIFO_SIZE];		
static uint32_t g_put_idx;				// index for putting data
static uint32_t g_get_idx;      		// index for getting data
static int32_t  g_fifo_curr_size;		// current size (0 is emtpy, 
										// FIFO_SIZE is full)

//*****************************************************************************
//
//  Prototypes for the private functions.
//
//*****************************************************************************

extern void run_os(void);	// defined in "osasm.s"
static void update_sleep_time(void);
static void init_task_stack(uint8_t task);
static void real_time_events(void);

//*****************************************************************************
//
//  Private Functions.
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief Set the initial stack of a task.
//!
//! @param[in] task .
//!
//! @return None.
//
//*****************************************************************************
static void init_task_stack(uint8_t task)
{
	g_tcbs[task].sp = &g_stacks[task][STACK_SIZE-16]; // task stack pointer
	g_stacks[task][STACK_SIZE-1] = 0x01000000;     // thumb bit
	g_stacks[task][STACK_SIZE-3] = 0x14141414;     // R14
	g_stacks[task][STACK_SIZE-4] = 0x12121212;     // R12
	g_stacks[task][STACK_SIZE-5] = 0x03030303;     // R3
	g_stacks[task][STACK_SIZE-6] = 0x02020202;     // R2
	g_stacks[task][STACK_SIZE-7] = 0x01010101;     // R1
	g_stacks[task][STACK_SIZE-8] = 0x00000000;     // R0
	g_stacks[task][STACK_SIZE-9] = 0x11111111;     // R11
	g_stacks[task][STACK_SIZE-10] = 0x10101010;    // R10
	g_stacks[task][STACK_SIZE-11] = 0x09090909;    // R9
	g_stacks[task][STACK_SIZE-12] = 0x08080808;    // R8
	g_stacks[task][STACK_SIZE-13] = 0x07070707;    // R7
	g_stacks[task][STACK_SIZE-14] = 0x06060606;    // R6
	g_stacks[task][STACK_SIZE-15] = 0x05050505;    // R5
	g_stacks[task][STACK_SIZE-16] = 0x04040404;    // R4
}

//*****************************************************************************
//
//! @brief Update sleep time.
//!
//! This function decreases the sleep time of sleeping tasks by one ms.
//!
//! @return None.
//
//*****************************************************************************
static void update_sleep_time(void)
{
	uint8_t i;
	for (i = 0; i < NUM_TASKS; i++)
	{
		if (g_tcbs[i].sleep)
		{
			g_tcbs[i].sleep--;
		}
	}
}

//*****************************************************************************
//
//! @brief Run periodic events.
//!
//! This function handles all the periodic events added by the user, in
//! addition to the internal event to update the sleep time of all tasks.
//!
//! @return None.
//
//*****************************************************************************
static void real_time_events(void)
{
	uint8_t i;
	bool call_scheduler = false;
	static uint32_t millis = 0;

	update_sleep_time();

	if(++millis)
	{
		for (i = 0; i < NUM_EVENTS; i++)
		{
			// only nonzero periods 
			if(g_ecbs[i].period)
			{
				if((millis % g_ecbs[i].period) == 0)
				{
					// signal the event semaphore so that
					// the task linked to it can run
					OS_Semaphore_post(g_ecbs[i].semaphore);
					call_scheduler = true;
				}
			}
		}

		// if an event has to run, call the scheduler
		if(call_scheduler)
		{
			OS_suspend();
		}
	}
}

//*****************************************************************************
//
//! @brief Scheduling algorithm.
//!
//! This fucntion runs once every time slice to choose the next process to run.
//! The scheduler uses a Fixed Priority algorithm to decide which task runs 
//! next. Calling function of the scheduler is inside of the SysTick_Handler
//! which was defined in the "osasm.s" file.
//!
//! @return None.
//
//*****************************************************************************
void scheduler(void)
{
	uint8_t max = 255;
	struct tcb *tmp;
	struct tcb *bst_task;
  	
	tmp = gp_running_task;

	// search for highest priority task that is not blocked or sleeping
	do
	{
		// move through the tasks available
		tmp = tmp->next;
		if((tmp->priority < max) && ((tmp->blocked) == 0) && ((tmp->sleep) == 0))
		{
			// keep track of the highest priority task 
			max = tmp->priority;
			bst_task = tmp;
		}
	} while(gp_running_task != tmp);

	gp_running_task = bst_task;
}

//*****************************************************************************
//
//  Functions for the API.
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief Start OS kernel.
//!
//! This function initializes the required hardware components of the TM4C123G 
//! to run the OS kernel.
//!
//! @return None.
//
//*****************************************************************************
void OS_start(void)
{
	uint32_t i, time_slice, events_period;
	const struct ecb ecb_default = 
	{
		.semaphore = 0, 
		.period = 0
	};

	// allowed period of time to run per task 
	time_slice = CPU_CLOCK_FREQ/TASK_FREQ;
	// running period for the real-time events
	events_period = CPU_CLOCK_FREQ/EVENT_FREQ;

	// disable interrupts
	CPU_disable_irq();
	// run CPU at 80 MHz			
	PLL_init();
	
	// enable wide timer 5 interrupt (WideTimer5A_Handler)
	// highest priority (0), period of interruption 1 ms 
	Timer_WTimer5A_init(events_period, 0);
	
	// enable system tick interrupt (SysTick_Handler)
	// lowest priority (7), period of interruption 1 ms  
	SysTick_init(time_slice, true, 7);
	
	// task 0 runs first
	gp_running_task = &g_tcbs[0]; 
	// uninitialized events are set with a default confing
	for (i = g_event_cnt; i < NUM_EVENTS; ++i)
	{
		g_ecbs[i] = ecb_default;
	}
	
	// run OS kernel (this function is defined in "osasm.s" file)
	run_os();
}

//*****************************************************************************
//
//! @brief Suspend the execution of the running task.
//!
//! This function allows the running task to release control of the CPU 
//! voluntarily by calling a low-level system function that forces the execution 
//! of the scheduler.
//!
//! @return None. 
//
//*****************************************************************************
void OS_suspend(void)
{
	// trigger SysTick interrupt (SysTick_Handler)
	SysTick_set_pending();
}

//*****************************************************************************
//
//! @brief Set the running task into sleep mode.
//!
//! This function sets the running task to sleep by changing the sleep filed 
//! of the TCB to a nonzero value and suspending its execution.
//!
//! @param[in] sleep_time Duration of the sleep in ms.
//!
//! @return None.
//
//*****************************************************************************
void OS_sleep(uint32_t sleep_time)
{
	// store in the TCB of the running task the sleep time
	gp_running_task->sleep = sleep_time;
	// release control of the CPU
	OS_suspend();
}

//*****************************************************************************
//
//! @brief Add task into the TCB array.
//!
//! This function initializes and adds a new task into the TCB array. No 
//! dynamic memory allocation used. 
//!
//! @param[in] p_task Pointer to the task function.
//! @param[in] priority Priority level of the task.
//!
//! @return 0 if successful, -1 if TCBs full.
//
//*****************************************************************************
int32_t OS_add_task(void (*p_task)(void), uint8_t priority)
{
	static uint8_t task_cnt = 0;

	if(task_cnt == NUM_TASKS)
	{
		return -1; // no additional space
	}

	// point to the next element (form a circular linked list)
	g_tcbs[task_cnt].next = &g_tcbs[task_cnt == 0 ? 0 : (task_cnt - 1)];
	g_tcbs[0].next = &g_tcbs[task_cnt];
	
	// not blocked, not sleep
	g_tcbs[task_cnt].blocked = 0;
	g_tcbs[task_cnt].sleep = 0;
	g_tcbs[task_cnt].priority = priority;

	// initilze task stack
	init_task_stack(task_cnt);
	// program counter (PC) points to the task function
	g_stacks[task_cnt][STACK_SIZE-2] = (uint32_t)(p_task);
	
	task_cnt++;

	return 0;
}

//*****************************************************************************
//
//! @brief Add a periodic event into the ECB array.
//!
//! This function initializes and adds a periodic event into the ECB array. No 
//! dynamic memory allocation used.
//!
//! @param[in] p_sema Pointer to an initialized semaphore.
//! @param[in] period Event execution period.
//!
//! @return 0 if successful, -1 if ECBs full.
//
//*****************************************************************************
int32_t OS_add_periodic_event(int32_t *p_sema, uint32_t period)
{
	if(g_event_cnt == NUM_EVENTS)
	{
		return -1; // no additional space
	}

	// add periodic event (only semaphores are supported)
	g_ecbs[g_event_cnt].semaphore = p_sema;
	g_ecbs[g_event_cnt].period = period;

	g_event_cnt++;

	return 0;
}

//*****************************************************************************
//
//! @brief Initialize semaphore.
//!
//! This function sets the inittial value of the semaphore.
//!
//! @param[in] p_sema Pointer to a semaphore.
//! @param[in] value Initial value of semaphore.
//!
//! @return None.
//
//*****************************************************************************
void OS_Semaphore_init(int32_t *p_sema, int32_t value)
{
	(*p_sema) = value;
}

//*****************************************************************************
//
//! @brief Wait for a semaphore.
//!
//! This function decrements a semaphore and blocks a task if less than zero.
//!
//! @param[in] p_sema Pointer to an initialized semaphore.
//!
//! @return None.
//
//*****************************************************************************
void OS_Semaphore_pend(int32_t *p_sema)
{
	// disable interrupts
	CPU_disable_irq();
	(*p_sema) = (*p_sema) - 1;

	// check if semaphore is occupied
	if((*p_sema) < 0)
	{
		// store reason of blocking
		gp_running_task->blocked = p_sema;
		CPU_enable_irq();
		// run scheduler
		OS_suspend();            
	}
	// enable interrupts
	CPU_enable_irq();
}

//*****************************************************************************
//
//! @brief Signal a semaphore.
//!
//! This function increments a semaphore and unblocks task if appropriate.
//!
//! @param[in] p_sema Pointer to an initialized semaphore.
//!
//! @return None.
//
//*****************************************************************************
void OS_Semaphore_post(int32_t *p_sema)
{
	struct tcb *tmp;

	// disable interrupts
	CPU_disable_irq();
	(*p_sema) = (*p_sema) + 1;
	
	// if there is a task blocked on this semaphore,
	// then find it and unblock it
	if((*p_sema) <= 0)
	{
		tmp = gp_running_task->next;
		while(tmp->blocked != p_sema)
		{
			tmp = tmp->next;
		}
		// unblock task
		tmp->blocked = 0; 
	}
	// enable interrupts
	CPU_enable_irq();
}

//*****************************************************************************
//
//! @brief Initialize fifo.
//!
//! This function sets the indieces for get and put operations to 
//! equal values, meaning that the fifo is empty.
//!
//! @return None.
//
//*****************************************************************************
void OS_Fifo_init(void)
{
	g_put_idx = 0, g_get_idx = 0;
	OS_Semaphore_init(&g_fifo_curr_size, 0);
}

//*****************************************************************************
//
//! @brief Put an entry into the fifo.
//!
//! This function inserts in a piece of data into the fifo as long as it is not 
//! not full. A unique semaphore is used to track the current size of the 
//! fifo.
//!
//!	@param[in] data Data entry to be inserted into the fifo.
//!
//!	@return 0 if successful, -1 if the fifo is full.
//
//*****************************************************************************
int8_t OS_Fifo_put(uint32_t data)
{
	if(g_fifo_curr_size == FIFO_SIZE)
	{		
		return -1; // fifo is full
	}
	
	// put data in the fifo and update index
	g_fifo[g_put_idx] = data;
	g_put_idx = g_put_idx < (FIFO_SIZE - 1) ? (g_put_idx + 1) : 0;
	// make data available
	OS_Semaphore_post(&g_fifo_curr_size);
	
	return 0;
}

//*****************************************************************************
//
//! @brief Get an entry from the fifo.
//!
//! This function retrieves a piece of data from the fifo as long as it is not 
//! not empty. A unique semaphore is used to track the current size of the 
//! fifo.
//!
//!	@return data retrieved.
//
//*****************************************************************************
uint32_t OS_Fifo_get(void)
{
	uint32_t data;

	// block if empty
	OS_Semaphore_pend(&g_fifo_curr_size);
	// get data and update index
	data = g_fifo[g_get_idx];        								
	g_get_idx = g_get_idx < (FIFO_SIZE - 1) ? (g_get_idx + 1) : 0;

	return data;
}

//*****************************************************************************
//
//  Interrupt Request (IRQ) Handlers.
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief IRQ Handler for the wide timer 5A.
//!
//!	This handler runs every millisecond to update any pending events.
//!
//!	@return None.
//
//*****************************************************************************
void WideTimer5A_Handler(void)
{
	// clear the interrupt flag
	Timer_WTimer5A_clear_irq();
	// runs events
	real_time_events();
}
