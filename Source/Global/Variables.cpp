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
	DeviceCreator	deviceCreator	= NULL;	// Used to create the Device
	Device			  device			  = NULL;	// Our rendering device
	LPD3DXFONT		font			    = NULL;	// For printing

	//-----------------------------------------------------------------------------
	// Global variables
	//-----------------------------------------------------------------------------
	LuaVM         lua_vm;
	DataLocator   data_locator;
	ScoreRecorder score_recorder;

	Frustum frustum;
	BackSideFrustum back_side_frustum;
	Fade    fade;

	TriggerManager   *triggers = NULL;
	CollisionManager collisions;
	SpatialManager   spatial;

	Player         *player = NULL;
	AnimatedObject  sky;   // El sky rota, por eso es animated
}
