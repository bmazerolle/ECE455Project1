/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

// Local includes
#include <CommonConfig.h>
#include <Display.h>
#include <Flow.h>
#include <Generator.h>
#include <Lights.h>

void middlewareSetup(void);
int queueSetup(void);
void taskSetup(void);
void timerSetup(void);

int main(void)
{
	middlewareSetup();
	if (queueSetup() != 0) return 1;
	taskSetup();
	timerSetup();

	// Start the system
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN);      // Green light on
	xTimerStart( xGreenLightSoftwareTimer, 0 );                     // Start the green light software timer

	uint16_t color = 1;
	xQueueOverwrite(xLightQueue, &color);							// Update the light status queue

	vTaskStartScheduler(); 											// Start the task scheduler

	return 0;
}

/*
 * Sets up the GPIO and ADC for interaction with the shift register, traffic light, and potentiometer
 */
void middlewareSetup ( void )
{
	// Declare the GPIO structures
	GPIO_InitTypeDef      SHIFT_GPIO_InitStructure;
	GPIO_InitTypeDef      TRAFFIC_GPIO_InitStructure;

	// Enable the GPIOC clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// Configure the shift register GPIO
    SHIFT_GPIO_InitStructure.GPIO_Pin   = SHIFT_RESET | SHIFT_CLOCK | SHIFT_DATA;
    SHIFT_GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    SHIFT_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    SHIFT_GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    SHIFT_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_Init(SHIFT_PORT, &SHIFT_GPIO_InitStructure);

    // Configure the traffic light GPIO
    TRAFFIC_GPIO_InitStructure.GPIO_Pin   = TRAFFIC_LIGHT_RED | TRAFFIC_LIGHT_YELLOW | TRAFFIC_LIGHT_GREEN;
    TRAFFIC_GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    TRAFFIC_GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    TRAFFIC_GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(TRAFFIC_LIGHT_PORT, &TRAFFIC_GPIO_InitStructure);

	// Declare the ADC structures
	ADC_InitTypeDef       ADC_InitStructure;
	GPIO_InitTypeDef      ADC_GPIO_InitStructure;

	// Enable GPIO and ADC clocks for ADC
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // Initialize ADC
    ADC_GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
    ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    ADC_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &ADC_GPIO_InitStructure);

    // Configure the ADC
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1, ENABLE);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_84Cycles);

    // Reset the clock and set the reset pin to clear the shift register
    GPIO_ResetBits(GPIOC, GPIO_Pin_7);
    GPIO_SetBits(GPIOC, GPIO_Pin_8);
}

/*
 * Initializes the necessary queues
 */
int queueSetup ( void )
{
	// Setup the shared queues
    xCarQueue = xQueueCreate(1, sizeof(uint16_t));
    if(xCarQueue == NULL) return 1; 				// Exit, failed to create
    vQueueAddToRegistry(xCarQueue, "xCarQueue");

    xFlowQueue = xQueueCreate(1, sizeof(uint16_t));
	if(xFlowQueue == NULL) return 1; 				// Exit, failed to create
	vQueueAddToRegistry(xFlowQueue, "xFlowQueue");

	xLightQueue = xQueueCreate(1, sizeof(uint16_t));
	if(xLightQueue == NULL) return 1; 				// Exit, failed to create
	vQueueAddToRegistry(xLightQueue, "xLightQueue");

	return 0;
}

/*
 * Initializes the tasks for traffic flow, creation, display, and traffic light display
 */
void taskSetup ( void )
{
	// Create the 4 tasks to be used
	xTaskCreate( TrafficFlowAdjustmentTask , "FlowAdjust",	configMINIMAL_STACK_SIZE ,	NULL ,	(tskIDLE_PRIORITY + 1)	,   NULL);
	xTaskCreate( TrafficCreatorTask        , "Creator"   ,	configMINIMAL_STACK_SIZE ,	NULL ,	(tskIDLE_PRIORITY + 2)	, 	NULL);
	xTaskCreate( TrafficLightTask          , "Light"	 ,	configMINIMAL_STACK_SIZE ,	NULL ,	(tskIDLE_PRIORITY + 2)	,  	NULL);
	xTaskCreate( TrafficDisplayTask        , "Display"   ,	configMINIMAL_STACK_SIZE ,	NULL ,	(tskIDLE_PRIORITY)		,	NULL);
}

/*
 * Initializes the software timers used for the traffic lights
 */
void timerSetup ()
{
	// Create the software timers for the traffic lights
	xGreenLightSoftwareTimer  = xTimerCreate("GreenLightTimer" ,   6000 / portTICK_PERIOD_MS , 	pdFALSE, ( void * ) 0,	vGreenLightTimerCallback);
	xYellowLightSoftwareTimer = xTimerCreate("YellowLightTimer",   2000 / portTICK_PERIOD_MS  , pdFALSE, ( void * ) 0,	vYellowLightTimerCallback);
	xRedLightSoftwareTimer    = xTimerCreate("RedLightTimer"   ,   6000 / portTICK_PERIOD_MS  , pdFALSE, ( void * ) 0,	vRedLightTimerCallback);
}

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

