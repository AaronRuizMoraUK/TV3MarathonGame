#ifndef DIGITAL_ACTION_H
#define DIGITAL_ACTION_H

#include <string>
#include <vector>
#include <Windows.h>

class MKeyValue;

enum InputDevices {KEYBOARD, /*MOUSE,*/ GAMEPAD, XBOX360GAMEPAD};

/**
* A configuration in an input device for a digital action
*/

struct DigitalActionConfigKey {
	InputDevices device;
	int          bitMask;
	std::string  name;
};

class DigitalActionConfigKeys : public std::vector<DigitalActionConfigKey *> {
public:
	const std::string & getKeyboardConfigKey( ) const;
	int getGamePadConfigKey( ) const;
	const std::string & getXBOX360GamePadConfigKey( ) const;
};

/**
* This is a digital action, press a button for example.
*/

class DigitalAction
{
protected:
	bool isPressedNow;
	bool wasPressed;
	float timePressed;

	DigitalActionConfigKeys digitalActionConfigKeys;

public:
	DigitalAction();
	~DigitalAction();

	const DigitalActionConfigKeys & getDigitalActionConfigKeys() const {
		return digitalActionConfigKeys;
	};

	bool isPressed() const;
	bool isReleased() const;
	bool becomesPressed() const;
	bool becomesReleased() const;

	void update(bool isPressed, float deltaTime);

	void onStartElement (const std::string &elem, MKeyValue &atts, std::ostringstream &outputStringStream, bool isWriteXMLEnabled=false);

	void setButton( InputDevices input, const std::string &buttonPressed );

	void unpress( ) {
		isPressedNow = wasPressed = false;
		timePressed = 0.0f;
	}
};

// Necesario para IOStatus
const std::string & getXBOX360KeyStringFromVK( WORD virtualKey );
int getXBOX360GamepadBitMask(const std::string &key);

#endif //DIGITAL_ACTION_H
