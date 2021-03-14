/*
 * 	TrafficCreator.c
 *  Deals with the creation of the next "car" LED
 */

#include <Generator.h>

/*
 * Creates the next car or empty space relative to the potentiometer input
 */
void TrafficCreatorTask ( void *pvParameters )
{
	uint16_t car = 0;
	uint16_t flow = 4;
	uint16_t sinceLastCar = 0;

	while(1)
	{
		xQueuePeek(xFlowQueue, &flow, 0);

		if (sinceLastCar > 5) { 							// If we haven't had a car in 5 spaces, guarantee one (per the requirements)
			car = 1;
			sinceLastCar = 0;
		} else {											// Otherwise, randomly generate a car with increasing probability as the ADC value increases
			car = (rand() % 100 ) < 100 / (8 - flow);		// A flow of 7 (maximum possible) guarantees traffic creation for bumper to bumper traffic
			if (car == 0) sinceLastCar++;
			else sinceLastCar = 0;
		}

		xQueueOverwrite(xCarQueue, &car);					// Write the 'next car' to the car queue to be pushed onto the shift register

		vTaskDelay(500);
	}
}
