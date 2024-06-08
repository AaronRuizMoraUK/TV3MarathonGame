#ifndef IPLAYER_CONTROLLER_H
#define IPLAYER_CONTROLLER_H

#include "Directx/Directx.h"
#include "Util/Signal/Signal.h"
#include "Managers/CollisionManager.h"

class Player;
class Object;

/**
* Player Controller
*/

class IPlayerController : public SignalListener
{
public:
	virtual ~IPlayerController() {}

	virtual const char *getControllerType () const = 0;

	virtual void update( Player &object, float deltaTime ) = 0;

	virtual void collision( Player &player, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal ) {}
};

#endif //IPLAYER_CONTROLLER_H
