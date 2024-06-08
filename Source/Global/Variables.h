#ifndef VARIABLES_H
#define VARIABLES_H

#include "Directx/Directx.h"
#include "Util/Frustum/Frustum.h"
#include "Util/Frustum/BackSideFrustum.h"
#include "Util/DataLocator/DataLocator.h"
#include "Util/ScoreRecorder/ScoreRecorder.h"
#include "Util/Fade/Fade.h"
#include "Object/AnimatedObject/Player/Player.h"
#include "Lua/LuaVM.h"
#include "Object/AnimatedObject/AnimatedObject.h"
#include "Managers/TriggerManager.h"
#include "Managers/CollisionManager.h"
#include "Managers/SpatialManager.h"

namespace Global {

	//-----------------------------------------------------------------------------
	// Global variables about Device
	//-----------------------------------------------------------------------------
	extern DeviceCreator	deviceCreator;	// Used to create the Device
	extern Device			    device;			    // Our rendering device
	extern LPD3DXFONT     font;			      // For printing

	//-----------------------------------------------------------------------------
	// Global variables
	//-----------------------------------------------------------------------------
	extern LuaVM         lua_vm;
	extern DataLocator   data_locator;
	extern ScoreRecorder score_recorder;

	extern Frustum         frustum;
	extern BackSideFrustum back_side_frustum;
	extern Fade            fade;

	extern TriggerManager   *triggers;
	extern CollisionManager collisions;
	extern SpatialManager   spatial;

	extern Player         *player;
	extern AnimatedObject  sky;   // El sky rota, por eso es animated
}

#endif //VARIABLES_H
