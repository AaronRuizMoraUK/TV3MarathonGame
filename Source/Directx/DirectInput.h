#ifndef DIRECT_INPUT_H
#define DIRECT_INPUT_H

#define DIRECTINPUT_VERSION 0x0800

#include "Directx.h"
#include <dinput.h>

//-----------------------------------------------------------------------------
// Definitions for XInput to manage any gamepad
//-----------------------------------------------------------------------------

// Types of devices to look for: all game controllers
#define GAMECONTROLLERS DI8DEVCLASS_GAMECTRL

// Max number of buttons allowed for gamepads
#define MAXBUTTONS 128

// Default to 35% of the +/- 1000 range.
// This is a reasonable default value but can be altered if needed.
#define INPUT_DEADZONE  ( 0.35f * FLOAT(0x3E8) )

// Enable or disable a dead zone for DirectInput gamepad
#define DEAD_ZONE_ON true

// Thumb min and max values given by DirectInput gamepad
#define THUMB_MIN_VALUE -1000
#define THUMB_MAX_VALUE 1000

typedef LPDIRECTINPUT8       DeviceInputCreator;
typedef LPDIRECTINPUTDEVICE8 DeviceInput;

HRESULT createDeviceInputCreator(DeviceInputCreator &deviceInputCreator);

// Direct input gamepad type and data (they are related)
#define TYPEINPUTGAMEPAD c_dfDIJoystick2
typedef DIJOYSTATE2 InputGamePad;

// Direct input gamepad data struct
struct GamePadData {
	BYTE buttons[MAXBUTTONS];

	D3DXVECTOR2 thumbLeft;
	D3DXVECTOR2 thumbRight;
};

#endif //DIRECT_INPUT_H
