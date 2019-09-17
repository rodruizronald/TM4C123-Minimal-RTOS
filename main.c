#include <stdint.h>
#include "os.h"

int32_t sema_A, sema_BC;
int32_t cnt_A, cnt_B, cnt_C, cnt_D, cnt_E, cnt_F, cnt_G;

//*****************************************************************************
//
//! task_A is a event-drivern task that runs every 10 ms. 
//
//*****************************************************************************
void task_A(void)
{ 
	cnt_A = 0;
	while(1)
	{
		OS_Semaphore_pend(&sema_A); // signaled by OS (periodic event)
		cnt_A++;
	}
}
        
//*****************************************************************************
//
//! Producer/Consumer without fifo
//! task_B is a data producer that runs periodically every 20 ms (sleep)
//! task_C is a data consumer that runs afters task_B finishes
//
//*****************************************************************************
void task_B(void)
{ 
	cnt_B = 0;
	while(1)
	{
		cnt_B++;
		OS_Semaphore_post(&sema_BC); // task c can be processed
		OS_sleep(20);
	}
}

void task_C(void)
{ 
	cnt_C = 0;
	while(1)
	{
		OS_Semaphore_pend(&sema_BC); // signaled by task b
		cnt_C = cnt_B;    
	}
}


//*****************************************************************************
//
//! Producer/Consumer with fifo
//! task_D is a data producer that runs periodically every 50 ms (sleep)
//! task_E is a data consumer that runs afters task_B finishes
//
//*****************************************************************************
void task_D(void)
{ 
	uint8_t i;
	cnt_D = 0;
	while(1)
	{
		// produce 5 data items
		for (i = 0; i < 5; i++)
		{
			OS_Fifo_put(++cnt_D);
		}
		OS_sleep(50);
	}
}

void task_E(void)
{ 
	cnt_E = 0;
	while(1)
	{
		// consume data items
		cnt_E = OS_Fifo_get();
	}
}

//*****************************************************************************
//
//! task_F is a lower level task that runs a lot
//
//*****************************************************************************
void task_F(void)
{ 
	cnt_F = 0; 
	while(1)
	{
		cnt_F++;
	}
}

int main(void)
{
	OS_Fifo_init();
	OS_Semaphore_init(&sema_A, 0);
	OS_Semaphore_init(&sema_BC, 0);
	// A is the highest priority task
	OS_add_task(&task_A, 0);
	OS_add_task(&task_B, 1);
	OS_add_task(&task_C, 2);
	OS_add_task(&task_D, 3);
	OS_add_task(&task_E, 4);
	OS_add_task(&task_F, 5);
	// event period is 10 ms
	OS_add_periodic_event(&sema_A, 10);

	OS_start();

	// this never executes
	return 0;             
}
