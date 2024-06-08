#ifndef ROA_CAMERA_H
#define ROA_CAMERA_H

#include "CameraControllers/ICameraController.h"
#include "Controllers/ROA.h"

/**
* Animation Controller for making camera follows
* a secuence of keyframed prerecorded animation
*/

class ROACamera: public ICameraController
{
protected:
	ROA roa;

public:
	ROACamera( const RigidAnimationResource * pRigidAnimationResource, bool loop = false );
	ROACamera( const RigidAnimationResource * pRigidAnimationResource
		, const D3DXVECTOR3 &initialPosition
		, const D3DXQUATERNION &initialRotation
		, bool loop = false );
	~ROACamera( );

	void update( Camera3D &camera, float deltaTime );
	bool isFinished() const { return roa.isFinished( ); }
};

#endif // ROA_CAMERA_H
