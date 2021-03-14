/*
 * 	TrafficDisplay.c
 *  Does the shifting logic for the car traffic
 */

#include <Display.h>

/*
 * 	Pushes the parameter (0 or 1) onto the shift register
 */
void ShiftRegisterPush( uint16_t newCar )
{
	if (newCar == 1) {                                          // Next space is a car, set the data bit high
		GPIO_SetBits(SHIFT_PORT, SHIFT_DATA);
	} else {                                                    // Next space is empty, set the data bit low
		GPIO_ResetBits(SHIFT_PORT, SHIFT_DATA);
	}

	GPIO_SetBits(SHIFT_PORT, SHIFT_CLOCK);        		// Pulse the clock to push the data bit on the shift register
	GPIO_ResetBits(SHIFT_PORT, SHIFT_CLOCK);
}

/*
 * Shifts the traffic forward and deals with the logic that controls traffic behavior relative to the lights
 */
void TrafficDisplayTask(void *pvParameters) {
	uint16_t car = 0;                            				// Default is no car
	uint16_t lightStatus = 1;                         			// Default light is green
	uint16_t previousTraffic[19] = { 0 }; 						// The state of traffic before any changes
	uint16_t newTraffic[19] = { 0 }; 							// The updated traffic to be pushed to the shift register

	while (1) {
		xQueuePeek(xCarQueue, &car, 0);							// Get the next car
		xQueuePeek(xLightQueue, &lightStatus, 0);				// Get the light status

		if (lightStatus == 1) {
			ShiftRegisterPush(car); 							// Push the next car onto the shift register
			newTraffic[0] = car; 								// Update the active car list with the new car value

			// Update the current traffic list with our new values
			for (uint16_t i = 1; i < 19; i++) {
				newTraffic[i] = previousTraffic[i - 1];
			}
		} else if (lightStatus == 0) {

			uint16_t spaceExists = 0; 							// Flag for free space in front of stop line

			// Check if the traffic before the stop line can be moved forward
			for (uint16_t i = 7; i != 0; i--) {
				if (previousTraffic[i] == 0) {
					spaceExists = 1;        					// Found a space, set the flag
					newTraffic[0] = car;
				}

				if (spaceExists == 1) {
					newTraffic[i] = previousTraffic[i - 1];		// Move the current car forward since there is space in front of it
				} else {
					newTraffic[i] = previousTraffic[i];			// Don't shift forward, no empty space yet
				}
			}

			newTraffic[8] = 0; 									// Update the LED directly after the stop line to be empty

			// Push all of the cars after the stop line forward one space
			for (uint16_t i = 18; i > 8; i--) {
				newTraffic[i] = previousTraffic[i - 1];
			}

			// Push the complete updated traffic array onto the shift register
			for (int16_t i = 18; i >= 0; i--) {
				ShiftRegisterPush(previousTraffic[i]);
			}
		}

		// Set the current traffic array to the new updated array
		for (uint16_t i = 0; i < 19; i++) {
			previousTraffic[i] = newTraffic[i];
		}

		vTaskDelay(500);
	}
}
