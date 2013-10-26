/*
    FreeRTOS V7.5.3 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

*/



/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Some of the RTOS hook (callback) functions only need special processing when
the full demo is being used.  The simply blinky demo has no special requirements,
so these functions are called from the hook functions defined in this file, but
are defined in main_full.c. */
void vFullDemoTickHookFunction( void );
void vFullDemoIdleFunction( void );

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName );
void vApplicationTickHook( void );

/* Demo task priorities. */
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainCOM_TEST_PRIORITY				( 2 )

/* The check task may require a bit more stack as it calls sprintf(). */
#define mainCHECK_TAKS_STACK_SIZE			( configMINIMAL_STACK_SIZE * 2 )

/* The execution period of the check task. */
#define mainCHECK_TASK_PERIOD				( ( portTickType ) 3000 / portTICK_RATE_MS )

/*-----------------------------------------------------------*/

static void vTask_test1(void *pvParameters);
static void vTask_test2(void *pvParameters);
static void vTask_test3(void *pvParameters);
static void vTask_test4(void *pvParameters);
static void vTask_test5(void *pvParameters);

/*-----------------------------------------------------------*/
xQueueHandle xQueue;

int main( void )
{
	/* Start the trace recording - the recording is written to a file if
	configASSERT() is called. */
	printf( "\r\nmain() started.\r\n" );

	xQueue = xQueueCreate(15, sizeof(int));

	xTaskCreate(vTask_test1, (signed char *)"T1", mainCHECK_TAKS_STACK_SIZE, NULL, 4, NULL);
	xTaskCreate( vTask_test2, ( signed char * )"T2", mainCHECK_TAKS_STACK_SIZE, NULL, 6, NULL );
	xTaskCreate( vTask_test3, ( signed char * )"T3", mainCHECK_TAKS_STACK_SIZE, NULL, 2, NULL );
	xTaskCreate(vTask_test4, (signed char *)"T4", mainCHECK_TAKS_STACK_SIZE, NULL, 5, NULL);

	/* Finally start the scheduler. */
	vTaskStartScheduler();

	return 0;
}


static void vTask_test3(void *pvParameters)
{
	BYTE a;
	portTickType xLastWakeTime;
	int value[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	portBASE_TYPE xStatus;

	(void)pvParameters;

	a = 0;
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, 200);
		
		for (a = 0; a < 10; a++)
		{
			xStatus = uxQueueMessagesWaiting(xQueue);
			printf("T3 QQQ ======= %d\n", xStatus);
			xStatus = xQueueSendToBack(xQueue, &value[a], 0);
			if (xStatus != pdPASS)
			{
				/* The send operation could not complete because the queue was full -
				this must be an error as the queue should never contain more than
				one item! */
				printf("xxxxxxxxxxxxx Could not send to the queue.\r\n");
			}

			printf("T3 =>Q [%d]\n", value[a]);
		}
	}
}

static void vTask_test4(void *pvParameters)
{
	BYTE a;
	portTickType xLastWakeTime;
	int value;
	portBASE_TYPE xStatus;

	(void)pvParameters;

	a = 0;
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		xStatus = xQueueReceive(xQueue, &value, portMAX_DELAY);
		if (xStatus != pdPASS)
		{
			/* The send operation could not complete because the queue was full -
			this must be an error as the queue should never contain more than
			one item! */
			printf("xxxxxxxxxxxxx Could not get from the queue.\r\n");
		}
		printf("T4 <=Q (%d)\n", value);
	}
}


static void vTask_test1(void *pvParameters)
{
	BYTE a;
	portTickType xLastWakeTime;

	(void)pvParameters;

	// this is test  git
	a = 0;
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, 100);
		// vTaskDelay( 2000 );
		printf("T1 %d\n", a);
		a = 1 - a;
	}
}

static void vTask_test2(void *pvParameters)
{
	BYTE a;
	portTickType xLastWakeTime;

	(void)pvParameters;

	a = 0;
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, 50);
		//vTaskDelay( 500 );
		printf("T2 %d\n", a);
		a = 2 - a;
	}
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */

	/* The trace can be stopped with any key press. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */

}
/*-----------------------------------------------------------*/

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
	/* Parameters are not used. */
	( void ) ulLine;
	( void ) pcFileName;

	taskDISABLE_INTERRUPTS();

	printf("\r\nx=x=x=x=x=x=x=x=x=x=x=x=x=x=x=x=x=\r\n");

	for( ;; );
}
/*-----------------------------------------------------------*/

