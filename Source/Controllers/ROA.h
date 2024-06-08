#ifndef ROA_H
#define ROA_H

#include "Directx/Directx.h"
#include "Object/Location.h"

class RigidAnimationResource;

/**
* Animation Controller for making animated objects follow
* a secuence of keyframed prerecorded animation
*/

class ROA
{
protected:
	const RigidAnimationResource * rigidAnimationResource;
	float currentTime;
	float timeScale;

	D3DXVECTOR3 position, positionSync;
	D3DXQUATERNION rotation, rotationSync;
	TLocation loc; // Se actualiza cuando se pide con getTransform

	bool loop;

public:
	ROA( const RigidAnimationResource * pRigidAnimationResource, bool ploop = false );
	ROA( const RigidAnimationResource * pRigidAnimationResource
		, const D3DXVECTOR3 &initialPosition
		, const D3DXQUATERNION &initialRotation
		, bool ploop = false );
	~ROA();

	void update( float deltaTime );
	bool isFinished() const;

	inline const D3DXVECTOR3 &getPosition() const { return position; };
	inline const D3DXQUATERNION &getRotation() const { return rotation; };
	TLocation &getLocation();

	inline void setTimeScale( float pTimeScale ) { timeScale = pTimeScale; };
	void restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation );

	void setPercentage( float percentage ); // Sin usar initialPosition ni initialRotation
};

#endif // KEYFRAMED_ROA_H
