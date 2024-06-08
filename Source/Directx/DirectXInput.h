#ifndef DIRECT_XINPUT_H
#define DIRECT_XINPUT_H

#include "Directx.h"
#include <XInput.h> // XInput API

//-----------------------------------------------------------------------------
// Definitions for XInput to manage the XBOX360 gamepad
//-----------------------------------------------------------------------------

// XInput handles up to 1 controllers 
#define MAX_CONTROLLERS_XBOX360 1

// Default to 35% of the +/- 32767 range.
// This is a reasonable default value but can be altered if needed.
#define INPUT_DEADZONE_XBOX360  ( 0.35f * FLOAT(0x7FFF) )

// Enable or disable a dead zone for XBOX360 gamepad
#define DEAD_ZONE_XBOX360_ON true

// Trigger min and max values given by XBOX360 gamepad
#define TRIGGER_XBOX360_MIN_VALUE 0.0f
#define TRIGGER_XBOX360_MAX_VALUE 255.0f

// Thumb min and max values given by XBOX360 gamepad
#define THUMB_XBOX360_MIN_VALUE -32768.0f
#define THUMB_XBOX360_MAX_VALUE 32768.0f

// Time that has to elapse to check if any XBOX360 gamepad has been connected 
#define TIME_CHECK_XBOX360 3.0f

// XBOX360 gamepad struct
struct GamePadXBOX360 {
	XINPUT_STATE    state;
	bool            isConnected;
};

// XBOX360 gamepad data struct
struct GamePadXBOX360Data {
	DWORD buttons;

	float leftTrigger;
	float rightTrigger;

	D3DXVECTOR2 thumbLeft;
	D3DXVECTOR2 thumbRight;
};

#endif //DIRECT_XINPUT_H
