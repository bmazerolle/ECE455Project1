/*
 * TrafficLight.c
 * Controls the state of the traffic lights
 */

#include <Lights.h>

/*
 * Called when green software timer expires and switches the light to yellow
 */
void vGreenLightTimerCallback( xTimerHandle xTimer )
{
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN);        // Green light off
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW);         // Yellow light on

	uint16_t color = 0;													// Update the current light status queue with the new light color
	xQueueOverwrite(xLightQueue, &color);

	xTimerStart( xYellowLightSoftwareTimer, 0 );                    	// Start the yellow light timer, yellow light callback will be hit at completion
}

/*
 * Called when yellow software timer expires and switches the light to red
 */
void vYellowLightTimerCallback( xTimerHandle xTimer )
{
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW);       // Yellow light off
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED);            // Red light on

	xTimerStart( xRedLightSoftwareTimer, 0 );                           // Start the red light timer, red light callback will be hit at completion
}

/*
 * Called when red software timer expires and switches the light to green
 */
void vRedLightTimerCallback( xTimerHandle xTimer )
{
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED);          // Red light off
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN);          // Green light on

	uint16_t color = 1;
	xQueueOverwrite(xLightQueue, &color);

	xTimerStart( xGreenLightSoftwareTimer, 0 );                     	// Start the green light timer, green light callback will be hit at completion
}


/*
 * Adjusts the traffic light durations if the traffic flow is changed
*/
void TrafficLightTask ( void *pvParameters )
{
	uint16_t newSpeed = 3;          								 	// Default speed (before ADC read) is 3 (scale of 0-7)
	uint16_t lastSpeed = 0;      						 				// Initially 0 to trigger the different speed conditional below

	while(1)
	{
		xQueuePeek(xFlowQueue, &newSpeed, 0); 							// Get the current speed from the queue

		if (lastSpeed != newSpeed) {
			uint16_t redSpeed = 4000 + 500 * (8 - newSpeed);			// Calculate the new red period
			uint16_t greenSpeed = 4000 + 500 * (newSpeed);				// Calculate the new green period

			if(xTimerIsTimerActive( xGreenLightSoftwareTimer )) {
				xTimerStop(xGreenLightSoftwareTimer, 0);                                                    // stop the green timer to change the period
				xTimerChangePeriod(xGreenLightSoftwareTimer, greenSpeed / portTICK_PERIOD_MS, 0 );  		// starts the green timer
				xTimerChangePeriod(xRedLightSoftwareTimer, redSpeed / portTICK_PERIOD_MS, 0 );     			// starts the red timer    (don't want it started)
				xTimerStop(xRedLightSoftwareTimer, 0);                                                      // stop the red timer
			} else if(xTimerIsTimerActive( xYellowLightSoftwareTimer )) {
				xTimerChangePeriod(xGreenLightSoftwareTimer, greenSpeed / portTICK_PERIOD_MS, 0 );  		// starts the green timer (don't want it started)
				xTimerStop(xGreenLightSoftwareTimer, 0);                                                    // stop the green timer
				xTimerChangePeriod(xRedLightSoftwareTimer, redSpeed / portTICK_PERIOD_MS, 0 );     			// starts the red timer    (don't want it started)
				xTimerStop(xRedLightSoftwareTimer, 0);                                                      // stop the red timer
			} else if(xTimerIsTimerActive( xRedLightSoftwareTimer )) {
				xTimerStop(xRedLightSoftwareTimer, 0);														// stop the red timer to change the period
				xTimerChangePeriod(xGreenLightSoftwareTimer, greenSpeed / portTICK_PERIOD_MS, 0 );  		// starts the green timer (don't want it started)
				xTimerStop(xGreenLightSoftwareTimer, 0);													// stop the green timer since red is on
				xTimerChangePeriod(xRedLightSoftwareTimer, redSpeed / portTICK_PERIOD_MS, 0 );     			// starts the red timer again
			}
		}

		lastSpeed = newSpeed; // update local speed variable

		vTaskDelay(500);
	}
}
