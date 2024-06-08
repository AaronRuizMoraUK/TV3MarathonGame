#ifndef IAI_CONTROLLER_H
#define IAI_CONTROLLER_H

#include "Directx/Directx.h"
#include "Util/Signal/Signal.h"
#include "Managers/CollisionManager.h"

class IntelligentObject;
class Object;

/**
* Interface of artificial intelligent controller for
* intelligent objects
*/

class IAIController : public SignalListener
{
public:
	virtual ~IAIController() {}

	virtual const char *getControllerType () const = 0;

	virtual bool update(IntelligentObject &object, float deltaTime) = 0;

	virtual void collision( IntelligentObject &int_object, const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal ) {}
};

#endif //IAI_CONTROLLER_H
