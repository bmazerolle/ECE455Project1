/*
 * Common variables shared between all tasks/main
 */

#ifndef COMMONCONFIG_H_
#define COMMONCONFIG_H_

// Library includes
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"


// Pin/Port definitions
#define ADC_PORT                GPIOC
#define ADC_PIN                 GPIO_Pin_3

#define SHIFT_PORT         	 	GPIOC
#define SHIFT_DATA          	GPIO_Pin_6
#define SHIFT_CLOCK         	GPIO_Pin_7
#define SHIFT_RESET 		    GPIO_Pin_8

#define TRAFFIC_LIGHT_PORT      GPIOC
#define TRAFFIC_LIGHT_RED    	GPIO_Pin_0
#define TRAFFIC_LIGHT_YELLOW 	GPIO_Pin_1
#define TRAFFIC_LIGHT_GREEN  	GPIO_Pin_2

QueueHandle_t xCarQueue;
QueueHandle_t xFlowQueue;
QueueHandle_t xLightQueue;

#endif /* COMMONCONFIG_H_ */

