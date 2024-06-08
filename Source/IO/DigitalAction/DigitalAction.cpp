#include <cassert>
#include <sstream>
#include "DigitalAction.h"
#include "Global/Rendering.h"
#include "Expat/XMLParser.h"
#include "Directx/DirectXInput.h"

int getKeyboardBitMask(const std::string &key);
//int getMouseBitMask(const std::string &key);
int getGamepadBitMask(const std::string &key);
//int getXBOX360GamepadBitMask(const std::string &key);
const std::string & getXBOX360LetterFromKeyString( const std::string &key );

// -------------------------------------------------------------------------------------

std::string keyString = "";

const std::string & DigitalActionConfigKeys::getKeyboardConfigKey( ) const {
	const_iterator it = begin( );
	while( it != end( ) ) {
		if( (*it)->device == KEYBOARD /*|| (*it)->device == MOUSE*/ ) {
			keyString = (*it)->name;
			return keyString;
		}

		++it;
	}

	keyString = "";
	return keyString;
}

int DigitalActionConfigKeys::getGamePadConfigKey( ) const {
	int keyInt = -1;

	const_iterator it = begin( );
	while( it != end( ) ) {
		if( (*it)->device == GAMEPAD ) {
			keyInt = (*it)->bitMask;
			return keyInt;
		}

		++it;
	}

	return keyInt;
}

const std::string & DigitalActionConfigKeys::getXBOX360GamePadConfigKey( ) const {
	const_iterator it = begin( );
	while( it != end( ) ) {
		if( (*it)->device == XBOX360GAMEPAD ) {
			keyString = getXBOX360LetterFromKeyString( (*it)->name );
			return keyString;
		}

		++it;
	}

	keyString = "";
	return keyString;
}

// ------------------------------------------------------------

DigitalAction::DigitalAction()
: isPressedNow(false)
, wasPressed(false)
, timePressed(0.0f)
{
}

DigitalAction::~DigitalAction() {
	printDebugString("Destroying Digital Action...\n");

	// Destroy Digital Action Config Keys
	while(!digitalActionConfigKeys.empty()) {
		size_t size = digitalActionConfigKeys.size()-1;
		DigitalActionConfigKey * configKey= digitalActionConfigKeys[size];
		if(configKey)
			delete configKey, configKey=NULL;
		digitalActionConfigKeys.pop_back();
	}
}

bool DigitalAction::isPressed() const {
	return isPressedNow;
}

bool DigitalAction::isReleased() const {
	return (!isPressedNow);
}

bool DigitalAction::becomesPressed() const {
	return (isPressedNow && !wasPressed);
}

bool DigitalAction::becomesReleased() const {
	return (!isPressedNow && wasPressed);
}

void DigitalAction::update(bool isPressed, float deltaTime){
	wasPressed = isPressedNow;
	isPressedNow = isPressed;

	if( isPressedNow && !wasPressed)
		timePressed=0.0f;
	else if ( isPressedNow && wasPressed)
		timePressed += deltaTime;
}

void DigitalAction::onStartElement (const std::string &elem, MKeyValue &atts, std::ostringstream &outputStringStream, bool isWriteXMLEnabled) {

	if (elem=="config") {

		if( isWriteXMLEnabled ) {
			const std::string &device=atts["device"];

			InputDevices inputDevice;
			if(device=="keyboard") {
				const std::string &key=atts["key"];
				if( key == "W" || key == "S" || key == "D" || key == "A" ||
					key == "UP" || key == "DOWN" || key == "RIGHT" || key == "LEFT" ) {
						atts.writeSingle( outputStringStream, elem.c_str() );
						return;
				}

				inputDevice = KEYBOARD;
			}
			else if(device=="gamepad")
				inputDevice = GAMEPAD;
			else if(device=="xbox360_gamepad")
				inputDevice = XBOX360GAMEPAD;
			else {
				assert( !"Device unknown in DigitalAction" );
				return;
			}

			DigitalActionConfigKeys::const_iterator it = digitalActionConfigKeys.begin( );
			while( it != digitalActionConfigKeys.end( ) ) {
				if( (*it)->device == inputDevice )
					break;
				++it;
			}
			assert( it != digitalActionConfigKeys.end( ) );

			atts.put( "key", (*it)->name.c_str( ) );

			atts.writeSingle( outputStringStream, elem.c_str() );
		}
		else {
			DigitalActionConfigKey *configKey = new DigitalActionConfigKey();

			const std::string &device=atts["device"];

			const std::string &key=atts["key"];
			if(key=="") {
				if(configKey)
					delete configKey, configKey=NULL;
				return;
			}

			configKey->name = key;
			if(device=="keyboard") {
				configKey->device=KEYBOARD;
				configKey->bitMask = getKeyboardBitMask(key);
			}
			/*
			else if (device=="mouse") {
			configKey->device=MOUSE;
			configKey->bitMask = getMouseBitMask(key);
			}
			*/
			else if (device=="gamepad") {
				configKey->device=GAMEPAD;
				configKey->bitMask = getGamepadBitMask(key);
			}
			else if (device=="xbox360_gamepad") {
				configKey->device=XBOX360GAMEPAD;
				configKey->bitMask = getXBOX360GamepadBitMask(key);
			}
			else {
				if(configKey)
					delete configKey, configKey=NULL;
				return;
			}

			if(configKey->bitMask==-1) {
				if(configKey)
					delete configKey, configKey=NULL;
				return;
			}

			/*char msg[ 128 ];
			sprintf_s(msg, sizeof(msg), "BitMask %s is 0x%02x\n", key, configKey->bitMask);
			printDebugString(msg);*/

			digitalActionConfigKeys.push_back(configKey);
		}
	}
}

void DigitalAction::setButton( InputDevices input, const std::string &buttonPressed ) {
	DigitalActionConfigKeys::iterator it = digitalActionConfigKeys.begin( );

	while( it != digitalActionConfigKeys.end( ) ) {
		DigitalActionConfigKey *configKey = (*it);

		if( configKey->device == input ) {
			configKey->name = buttonPressed;

			if( input == KEYBOARD ) {
				configKey->bitMask = getKeyboardBitMask( buttonPressed );
			}
			else if( input == GAMEPAD ) {
				configKey->bitMask = getGamepadBitMask( buttonPressed );
			}
			else if( input == XBOX360GAMEPAD ) {
				configKey->bitMask = getXBOX360GamepadBitMask( buttonPressed );
			}
			else {
				assert( !"Input unknown in DigitalAction" );
				return;
			}

			return;
		}

		++it;
	}

	// If we are here means that kind of input is not defined here, so add it
	DigitalActionConfigKey *configKey = new DigitalActionConfigKey();
	configKey->device = input;
	configKey->name = buttonPressed;

	if( input == KEYBOARD ) {
		configKey->bitMask = getKeyboardBitMask( buttonPressed );
	}
	else if( input == GAMEPAD ) {
		configKey->bitMask = getGamepadBitMask( buttonPressed );
	}
	else if( input == XBOX360GAMEPAD ) {
		configKey->bitMask = getXBOX360GamepadBitMask( buttonPressed );
	}
	else {
		assert( !"Input unknown in DigitalAction" );
		return;
	}
}

// ----------------------------------------------------------------------------------------

/*
int getMouseBitMask(const std::string &key) {
if(key=="MOUSE1")
return VK_LBUTTON;
else if(key=="MOUSE2")
return VK_RBUTTON;
else if(key=="MOUSE3")
return VK_MBUTTON;

return -1;
}
*/

int getGamepadBitMask(const std::string &key) {
	if(key=="BUTTON0")
		return 0;
	else if(key=="BUTTON1")
		return 1;
	else if(key=="BUTTON2")
		return 2;
	else if(key=="BUTTON3")
		return 3;
	else if(key=="BUTTON4")
		return 4;
	else if(key=="BUTTON5")
		return 5;
	else if(key=="BUTTON6")
		return 6;
	else if(key=="BUTTON7")
		return 7;
	else if(key=="BUTTON8")
		return 8;
	else if(key=="BUTTON9")
		return 9;
	else if(key=="BUTTON10")
		return 10;
	else if(key=="BUTTON11")
		return 11;
	else if(key=="BUTTON12")
		return 12;
	else if(key=="BUTTON13")
		return 13;
	else if(key=="BUTTON14")
		return 14;
	else if(key=="BUTTON15")
		return 15;

	return -1;
}

int getXBOX360GamepadBitMask(const std::string &key) {
	if(key=="A")
		return XINPUT_GAMEPAD_A;
	else if(key=="B")
		return XINPUT_GAMEPAD_B;
	else if(key=="X")
		return XINPUT_GAMEPAD_X;
	else if(key=="Y")
		return XINPUT_GAMEPAD_Y;
	else if(key=="LEFT_SHOULDER")
		return XINPUT_GAMEPAD_LEFT_SHOULDER;
	else if(key=="RIGHT_SHOULDER")
		return XINPUT_GAMEPAD_RIGHT_SHOULDER;
	else if(key=="DPAD_UP")
		return XINPUT_GAMEPAD_DPAD_UP;
	else if(key=="DPAD_DOWN")
		return XINPUT_GAMEPAD_DPAD_DOWN;
	else if(key=="DPAD_LEFT")
		return XINPUT_GAMEPAD_DPAD_LEFT;
	else if(key=="DPAD_RIGHT")
		return XINPUT_GAMEPAD_DPAD_RIGHT;
	else if(key=="LEFT_TRIGGER")
		return VK_PAD_LTRIGGER;
	else if(key=="RIGHT_TRIGGER")
		return VK_PAD_RTRIGGER;
	else if(key=="START")
		return XINPUT_GAMEPAD_START;
	else if(key=="BACK")
		return XINPUT_GAMEPAD_BACK;

	return -1;
}

std::string keyStringXBOX = "";
const std::string & getXBOX360KeyStringFromVK( WORD virtualKey ) {

	if( virtualKey == VK_PAD_LSHOULDER )
		keyStringXBOX = "LEFT_SHOULDER";
	else if ( virtualKey == VK_PAD_RSHOULDER )
		keyStringXBOX = "RIGHT_SHOULDER";
	else if ( virtualKey == VK_PAD_LTRIGGER )
		keyStringXBOX = "LEFT_TRIGGER";
	else if ( virtualKey == VK_PAD_RTRIGGER )
		keyStringXBOX = "RIGHT_TRIGGER";
	else if ( virtualKey == VK_PAD_START )
		keyStringXBOX = "START";
	else if ( virtualKey == VK_PAD_A ) 
		keyStringXBOX = "A";
	else if ( virtualKey == VK_PAD_B ) 
		keyStringXBOX = "B";
	else if ( virtualKey == VK_PAD_X ) 
		keyStringXBOX = "X";
	else if ( virtualKey == VK_PAD_Y ) 
		keyStringXBOX = "Y";
	else {
		keyStringXBOX = "";
	}

	return keyStringXBOX;
}

// Devuelve la cadena necesaria para usar el string renderer de los botones de la xbox
const std::string & getXBOX360LetterFromKeyString( const std::string &key ) {

	if( key == "LEFT_SHOULDER" )
		keyStringXBOX = "n";
	else if ( key == "RIGHT_SHOULDER" )
		keyStringXBOX = "m";
	else if ( key == "LEFT_TRIGGER" )
		keyStringXBOX = "l";
	else if ( key == "RIGHT_TRIGGER" )
		keyStringXBOX = "r";
	else if ( key == "START" )
		keyStringXBOX = "s";
	else if ( key == "A" || 
			  key == "B" || 
			  key == "X" || 
			  key == "Y" ) {
		return key;
	}
	else {
		assert( !"XBOX360 key button string unknown in XBOXButtonsRender" );
		keyStringXBOX = "";
		return keyStringXBOX;
	}

	return keyStringXBOX;
}

int getKeyboardBitMask(const std::string &key) {
	if(key.size()==1)
		return  key.c_str()[0];

	else if (key=="UP")
		return VK_UP;
	else if (key=="DOWN")
		return VK_DOWN;
	else if (key=="RIGHT")
		return VK_RIGHT ;
	else if (key=="LEFT")
		return VK_LEFT;

	else if(key=="MOUSE1")
		return VK_LBUTTON;
	else if(key=="MOUSE2")
		return VK_RBUTTON;
	else if(key=="MOUSE3")
		return VK_MBUTTON;

	else if (key=="ENTER")
		return VK_RETURN;
	else if (key=="SPACE")
		return VK_SPACE;
	else if (key=="ESCAPE")
		return VK_ESCAPE;
	else if (key=="TAB")
		return VK_TAB;
	else if (key=="BACK")
		return VK_BACK;
	else if (key=="CAPITAL")
		return VK_CAPITAL;

	else if (key=="SHIFT")
		return VK_SHIFT;
	else if (key=="CONTROL")
		return VK_CONTROL;
	else if (key=="ALT")
		return VK_MENU;
	else if (key=="LSHIFT")
		return VK_LSHIFT;
	else if (key=="RSHIFT")
		return VK_RSHIFT ;
	else if (key=="LCONTROL")
		return VK_LCONTROL;
	else if (key=="RCONTROL")
		return VK_RCONTROL ;

	else if (key=="INSERT")
		return VK_INSERT ;
	else if (key=="DELETE")
		return VK_DELETE;
	else if (key=="PRIOR")
		return VK_PRIOR;
	else if (key=="NEXT")
		return VK_NEXT;
	else if (key=="END")
		return VK_END;
	else if (key=="HOME")
		return VK_HOME;
	else if (key=="PRINT")
		return VK_PRINT;

	else if (key=="F1")
		return VK_F1;
	else if (key=="F2")
		return VK_F2;
	else if (key=="F3")
		return VK_F3;
	else if (key=="F4")
		return VK_F4;
	else if (key=="F5")
		return VK_F5;
	else if (key=="F6")
		return VK_F6;
	else if (key=="F7")
		return VK_F7;
	else if (key=="F8")
		return VK_F8;
	else if (key=="F9")
		return VK_F9;
	else if (key=="F10")
		return VK_F10;
	else if (key=="F11")
		return VK_F11;
	else if (key=="F12")
		return VK_F12;

	return -1;
}

// ----------------------------------------------------------------------------------------
