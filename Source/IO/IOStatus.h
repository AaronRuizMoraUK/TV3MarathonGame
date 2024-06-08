#ifndef IO_STATUS_H
#define IO_STATUS_H

#include <sstream>
#include "XMLParser\XMLParser.h"
#include "DigitalAction\DigitalAction.h"
#include "AnalogAction\AnalogAction.h"
#include "Directx\DirectInput.h"
#include "Directx\DirectXInput.h"
#include "Util\Timeout\Timeout.h"

/**
* Input/Output status. This stores the status of
* the input devices in digital and analog actions.
*/

//NOTE: The DirectInput gamepads only works if they are plugged
//      before the game runs.

class IOStatus : public CXMLParser
{
public:
	enum Actions {JUMP, KICK, WALK, PAUSE, NUM_ACTIONS};

	DigitalAction jump;
	DigitalAction kick;
	DigitalAction start;
	DigitalAction selectOption;
	DigitalAction enterOptions;
	DigitalAction exitOptions;

	DigitalAction walkSlow;  // Only from keyboard

	AnalogAction walk;

	// Just For Debug
	bool xboxBecomesPressed( std::string xbox_button ) const;
	bool xboxBecomesReleased( std::string xbox_button ) const;
	bool xboxIsPressed( std::string xbox_button ) const;
	bool xboxIsReleased( std::string xbox_button ) const;
	const D3DXVECTOR2 &xboxGetThumbLeftDirection( ) const;
	const D3DXVECTOR2 &xboxGetThumbRightDirection( ) const;

protected:
	static std::string pathToXMLs;

	// Digital actions for analog actions
	DigitalAction walkUp;    // Only from keyboard
	DigitalAction walkDown;  // Only from keyboard
	DigitalAction walkRight; // Only from keyboard
	DigitalAction walkLeft;  // Only from keyboard

private:
	// Used to write in xml
	std::ostringstream outputStringStream;
	bool isWriteXMLEnabled;
	
	bool input_disabled;

public:
	IOStatus();
	~IOStatus();

	void initiate(HWND hWnd);

	void update(float deltaTime);

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);
	void writeXML( );

	bool detectButtonOnInput( InputDevices input, std::string &buttonPressed ) const;
	void setButton( InputDevices input, Actions action, const std::string &buttonPressed );

	void enableInput( );
	void disableInput( );
	bool isInputDisabled( ) const { return input_disabled; }

private:
	static DeviceInputCreator deviceInputCreator;
	static DeviceInput gamePad;
	GamePadXBOX360 gamePadsXBOX360[MAX_CONTROLLERS_XBOX360];

	GamePadXBOX360Data prev_gamePadXBOX360Data;
	GamePadXBOX360Data gamePadXBOX360Data;
	GamePadData gamePadData;

	int numGamePadsXBOX360Connected;
	int numGamePadsConnected;

	DigitalAction *currentControl; // This is used just for reading XML

	Timeout timeoutCheckGamePadXBOX360;

private:
	HRESULT createDirectInput(HWND hWnd);
	HRESULT createDeviceInputGamePad(HWND hWnd);
	void releaseDirectInput();

	void updateGamePadsState();
	void readGamePadData();

	void updateGamePadsXBOX360State(float deltaTime);
	void readGamePadXBOX360Data();

	void processData(float deltaTime);
	void updateDigitalAction(DigitalAction &digitalAction, float deltaTime);

	friend BOOL CALLBACK EnumGamePadsCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
	friend BOOL CALLBACK ScaleAxisGamePadsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
};

extern IOStatus ioStatus;

bool isPushed(int vKey);

#endif //IO_STATUS_H
