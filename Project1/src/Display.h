#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <CommonConfig.h>

void ShiftRegisterPush( uint16_t newCar );
void TrafficDisplayTask( void *pvParameters );

#endif /* DISPLAY_H_ */
