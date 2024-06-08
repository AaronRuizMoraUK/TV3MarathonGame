#ifndef ROA_OBJECT_H
#define ROA_OBJECT_H

#include "AnimationControllers/IAnimationController.h"
#include "Controllers/ROA.h"

/**
* Animation Controller for making animated objects follow
* a secuence of keyframed prerecorded animation
*/

class ROAObject: public IAnimationController
{
protected:
	ROA roa;

public:
	ROAObject( const RigidAnimationResource * pRigidAnimationResource, bool loop = false );
	ROAObject( const RigidAnimationResource * pRigidAnimationResource
		, const D3DXVECTOR3 &initialPosition
		, const D3DXQUATERNION &initialRotation
		, bool loop = false );
	~ROAObject( );

	void update( AnimatedObject &object, float deltaTime );
	bool isFinished() const { return roa.isFinished( ); }

	inline void setTimeScale( float pTimeScale ) { roa.setTimeScale( pTimeScale ); }

	const char * getControllerType () const { return "ROAObject"; }
	
	inline void setPercentage( float percentage ) { roa.setPercentage( percentage ); }
};

#endif // ROA_OBJECT_H
