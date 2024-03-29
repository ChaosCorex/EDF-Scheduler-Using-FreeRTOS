/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );

#if ( configUSE_EDF_SCHEDULER == 1 )
    BaseType_t xTaskPeriodicCreate( TaskFunction_t pxTaskCode,
                                                    const char * const pcName,
                                                    const configSTACK_DEPTH_TYPE usStackDepth,
                                                    void * const pvParameters,
                                                    UBaseType_t uxPriority,
                                                    TaskHandle_t * const pxCreatedTask,
                                                    TickType_t period    );
#endif
																										
TaskHandle_t xButton_1_Handler, xButton_2_Handler, xUART_Sender_Handler, xUART_Reciever_Handler, xLoad_1_Handler, xLoad_2_Handler;

int task_1_in_time = 0, task_1_out_time = 0, task_1_total_time=0, cpu_load_1;
int task_2_in_time = 0, task_2_out_time = 0, task_2_total_time=0, cpu_load_2;
int task_3_in_time = 0, task_3_out_time = 0, task_3_total_time=0, cpu_load_3;
int task_4_in_time = 0, task_4_out_time = 0, task_4_total_time=0, cpu_load_4;
int task_5_in_time = 0, task_5_out_time = 0, task_5_total_time=0, cpu_load_5;
int task_6_in_time = 0, task_6_out_time = 0, task_6_total_time=0, cpu_load_6;
int system_time =0;
int cpu_load = 0;
uint8_t dataBuffer;
/*-----------------------------------------------------------*/

/*This task will monitor rising and falling edge on button 1 and send this event to the consumer task.
(Note: The rising and failling edges are treated as separate events, hence they have separate strings*/
void vTask_1(void * pvParameters ){
    TickType_t pxPreviousWakeTime;
    vTaskSetApplicationTaskTag( NULL, ( void *) 1 );
    for(;;){
        pxPreviousWakeTime = xTaskGetTickCount();
        if(GPIO_read(PORT_0,PIN8))
            while(GPIO_read(PORT_0,PIN8)); // polling
        vTaskDelayUntil(&pxPreviousWakeTime, 50);
    }
}
/*This task will monitor rising and falling edge on button 2 and send this event to the consumer task. 
(Note: The rising and failling edges are treated as separate events, hence they have separate strings*/
void vTask_2(void * pvParameters ){
    TickType_t pxPreviousWakeTime;
    vTaskSetApplicationTaskTag( NULL, ( void *) 2 );
    for(;;){
        pxPreviousWakeTime = xTaskGetTickCount();
                if(GPIO_read(PORT_0,PIN9))
                while(GPIO_read(PORT_0,PIN9));// polling
        vTaskDelayUntil(&pxPreviousWakeTime, 50);
    }
}
/* This task will send preiodic string every 100ms to the consumer task*/
void vTask_3(voidpvParameters ){
    TickType_t pxPreviousWakeTime=0;
    vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
    for(;;){
        task_3_in_time = portGET_RUN_TIME_COUNTER_VALUE();
        pxPreviousWakeTime = xTaskGetTickCount();
        dataBuffer = 'A';
        vTaskDelayUntil(&pxPreviousWakeTime, 100);
    }
}

/* This is the consumer task which will write on UART any received string from other tasks*/
void vTask_4(voidpvParameters ){
    TickType_t pxPreviousWakeTime;
    vTaskSetApplicationTaskTag( NULL, ( void * ) 4 );
    for(;;){
        pxPreviousWakeTime = xTaskGetTickCount();
        xSerialPutChar(dataBuffer);
        vTaskDelayUntil(&pxPreviousWakeTime, 20);
    }
}

void vTask_5(voidpvParameters ){
		int i = 0;
    TickType_t pxPreviousWakeTime;
    vTaskSetApplicationTaskTag( NULL, ( void * ) 5 );
    for(;;){
			pxPreviousWakeTime = xTaskGetTickCount();
        for(i=0;i<33330;i++){i=i;}
			vTaskDelayUntil(&pxPreviousWakeTime, 10);
    }
}

void vTask_6(voidpvParameters ){
		int i = 0;
    TickType_t pxPreviousWakeTime;
    vTaskSetApplicationTaskTag( NULL, ( void * ) 6 );
    for(;;){
        pxPreviousWakeTime = xTaskGetTickCount();
        for(i=0;i<79992;i++){i=i;}
			vTaskDelayUntil(&pxPreviousWakeTime, 100);
    }
}
/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
    /* Create Tasks here */
 /* Create the task, storing the handle. */
    xTaskPeriodicCreate(
                    vTask_1,       /* Function that implements the task. */
                    "Button 1 Task",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xButton_1_Handler, 
										50);      /* Used to pass out the created task's handle. */
										
		xTaskPeriodicCreate(
                    vTask_2,       /* Function that implements the task. */
                    "Button 2 Task",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xButton_2_Handler,
										50);      /* Used to pass out the created task's handle. */
		xTaskPeriodicCreate(
                    vTask_3,       /* Function that implements the task. */
                    "UART Send Task",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xUART_Sender_Handler,
										100);      /* Used to pass out the created task's handle. */
		xTaskPeriodicCreate(
                    vTask_4,       /* Function that implements the task. */
                    "UART Recieve Task",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xUART_Reciever_Handler,
										20);      /* Used to pass out the created task's handle. */
		xTaskPeriodicCreate(
                    vTask_5,       /* Function that implements the task. */
                    "Load 1 Task",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xLoad_1_Handler,
										10);      /* Used to pass out the created task's handle. */
		xTaskPeriodicCreate(
                    vTask_6,       /* Function that implements the task. */
                    "Load 2 Task",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xLoad_2_Handler,
										100);      /* Used to pass out the created task's handle. */								

										
	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/
void vApplicationIdleHook(){
	
}
void vApplicationTickHook(void){
        GPIO_write(PORT_0, PIN0, PIN_IS_HIGH);
        GPIO_write(PORT_0, PIN0, PIN_IS_LOW);
}
/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


