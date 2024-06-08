#include "AnalogAction.h"
#include "Global/Rendering.h"

#define Y_UMBRAL 0.8f
#define X_UMBRAL 0.8f

AnalogAction::AnalogAction()
: direction(0,0)
, upIsPressedNow( false )
, upWasPressed( false )
, downIsPressedNow( false )
, downWasPressed( false )
, rightIsPressedNow( false )
, rightWasPressed( false )
, leftIsPressedNow( false )
, leftWasPressed( false )
{}

AnalogAction::~AnalogAction() {
	printDebugString("Destroying Analog Action...\n");
}

bool AnalogAction::isPressedUp() const {
	return upIsPressedNow;
}

bool AnalogAction::isPressedDown() const {
	return downIsPressedNow;
}

bool AnalogAction::isPressedRight() const {
	return rightIsPressedNow;
}

bool AnalogAction::isPressedLeft() const {
	return leftIsPressedNow;
}

bool AnalogAction::isReleasedUp() const {
	return (!upIsPressedNow);
}

bool AnalogAction::isReleasedDown() const {
	return (!downIsPressedNow);
}

bool AnalogAction::isReleasedRight() const {
	return (!rightIsPressedNow);
}

bool AnalogAction::isReleasedLeft() const {
	return (!leftIsPressedNow);
}

bool AnalogAction::becomesPressedUp() const {
	return (upIsPressedNow && !upWasPressed);
}

bool AnalogAction::becomesPressedDown() const {
	return (downIsPressedNow && !downWasPressed);
}

bool AnalogAction::becomesPressedRight() const {
	return (rightIsPressedNow && !rightWasPressed);
}

bool AnalogAction::becomesPressedLeft() const {
	return (leftIsPressedNow && !leftWasPressed);
}

bool AnalogAction::becomesReleasedUp() const {
	return (!upIsPressedNow && upWasPressed);
}

bool AnalogAction::becomesReleasedDown() const {
	return (!downIsPressedNow && downWasPressed);
}

bool AnalogAction::becomesReleasedRight() const {
	return (!rightIsPressedNow && rightWasPressed);
}

bool AnalogAction::becomesReleasedLeft() const {
	return (!leftIsPressedNow && leftWasPressed);
}

void AnalogAction::update(D3DXVECTOR2 &pDirection) {
	direction=pDirection;

	upWasPressed = upIsPressedNow;
	upIsPressedNow = checkUp();

	downWasPressed = downIsPressedNow;
	downIsPressedNow = checkDown();

	rightWasPressed = rightIsPressedNow;
	rightIsPressedNow = checkRight();

	leftWasPressed = leftIsPressedNow;
	leftIsPressedNow = checkLeft();
}

bool AnalogAction::checkUp() const {
	return ( direction.y > Y_UMBRAL );
}

bool AnalogAction::checkDown() const {
	return ( direction.y < -Y_UMBRAL );
}

bool AnalogAction::checkRight() const {
	return ( direction.x > X_UMBRAL );
}

bool AnalogAction::checkLeft() const {
	return ( direction.x < -X_UMBRAL );
}
