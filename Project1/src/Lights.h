#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <CommonConfig.h>

//Timer declarations
void vGreenLightTimerCallback( xTimerHandle xTimer );
void vYellowLightTimerCallback( xTimerHandle xTimer );
void vRedLightTimerCallback( xTimerHandle xTimer );

void TrafficLightTask( void *pvParameters );

xTimerHandle xRedLightSoftwareTimer;
xTimerHandle xYellowLightSoftwareTimer;
xTimerHandle xGreenLightSoftwareTimer;

#endif /* LIGHTS_H_ */
