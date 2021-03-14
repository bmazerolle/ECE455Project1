/*
 * TrafficFlow.c
 * Converts the Potentiometer value into a digital flow value from 0-7 and updates the flow queue
 */

#include <Flow.h>

/*
* Converts the analog potentiometer value to a digital value and updates the flow queue
*/
void TrafficFlowAdjustmentTask ( void *pvParameters )
{
    uint16_t analogFlow = 0;            					// Default analog reading is 0
    uint16_t digitalFlow = 0;								// Default digital conversion is 0
    uint16_t previousAnalogFlow = 0;								// Default previous digital conversion is 0
    uint16_t flowDifference;

	while(1)
	{
		ADC_SoftwareStartConv(ADC1);		            	// Initiate ADC conversion
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));		// Wait for conversion to finish
		analogFlow = ADC_GetConversionValue(ADC1);       	// Get the converted value

		flowDifference = abs(analogFlow - previousAnalogFlow);   // See if there is a flow change

		digitalFlow = analogFlow / 512;                		// Divide the analog value by 512 to produce an int from 0-7
		if (digitalFlow == 8) digitalFlow = 7;				// Will only be 8 if at max capacity

	    if (flowDifference > 100) {
	    	previousAnalogFlow = analogFlow;				// Update the local flow variable and the shared queue
	    	xQueueOverwrite(xFlowQueue, &digitalFlow);
	    }

        vTaskDelay(250);
	}
}
