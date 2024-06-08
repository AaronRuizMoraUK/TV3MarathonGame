#include "Timeout.h"
#include "Global/Rendering.h"

Timeout::Timeout()
: timeToElapse(0.0f)
, elapsedTime(0.0f)
{}

Timeout::Timeout(float pTimeToElapse)
: timeToElapse(pTimeToElapse)
, elapsedTime(0.0f)
{
}

Timeout::~Timeout() {
	printDebugString("Destroying Timeout...\n");
}

void Timeout::init(float pTimeToElapse)
{
	timeToElapse = pTimeToElapse;
	elapsedTime = 0.0f;
}
