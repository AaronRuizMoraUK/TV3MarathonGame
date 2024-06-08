#ifndef INC_LOCATION_H_
#define INC_LOCATION_H_

#include "Directx/Directx.h"
#include "Solid/SOLID.h"

struct ShapeHandle;

class TLocation {
protected:
	D3DXVECTOR3 position;
	D3DXVECTOR3 localFront;
	D3DXVECTOR3 localLeft;
	D3DXVECTOR3 localUp;
	D3DXMATRIX transform;

	D3DXMATRIX inverse_transform; // Calculado cada vez que se llama a getInverseTransform()
	D3DXQUATERNION rotation;    // Calculado cada vez que se llama a getRotation()

	// Alpha and delta angles with respect planet
	float alpha;
	float delta;

	// Solo para casos especiales donde un objecto se encuentra encima de algo
	const TLocation *parent_loc;
	D3DXMATRIX local_transform;

public:
	TLocation();
	virtual ~TLocation();
	
	inline const D3DXVECTOR3 & getPosition() const {return position;}
	virtual void setPosition(const D3DXVECTOR3 &a_position);

	inline const D3DXVECTOR3 & getLocalLeft() const {return localLeft;}
	inline const D3DXVECTOR3 & getLocalUp() const {return localUp;}
	inline const D3DXVECTOR3 & getLocalFront() const {return localFront;}

	const D3DXQUATERNION & getRotation();
	inline const D3DXMATRIX & getTransform() const { return transform; }
	virtual void setTransform(const D3DXMATRIX &a_transform);

	inline const float getAlpha( ) const { return alpha; }
	inline const float getDelta( ) const { return delta; }
	inline void setAlpha( float palpha ) { alpha = palpha; }
	inline void setDelta( float pdelta ) { delta = pdelta; }

	const D3DXMATRIX & getInverseTransform();

	// Solo para casos especiales donde un objecto se encuentra encima de algo
	void setParentLocation( const TLocation *loc ) { parent_loc = loc; }
	void setLocalTransform( const D3DXMATRIX &alocal_transform ) { local_transform = alocal_transform; }
	void updateTransformWithParent( );

private:
	void updateVariablesFromTransform();
};

// -------------------------------------------------------------------------------------

/**
* Location for make the object a collisionable object in scene.
* This has a Solid shape and an object.
*/

class LocationCollisionable : public TLocation {
protected:
	const ShapeHandle *collisionShapeHandle;
	DT_ObjectHandle    collisionObjectHandle;

	// Previous transform for collision
	D3DXMATRIX prev_transform;

public:
	LocationCollisionable();
	LocationCollisionable(const LocationCollisionable &source);
	~LocationCollisionable();

	void operator=(const LocationCollisionable &source);

	void createCollisionObject( const ShapeHandle *shapeHandle );
	inline const DT_ObjectHandle getCollisionObject() const { return collisionObjectHandle; }

	void setPosition(const D3DXVECTOR3 &a_position);
	void setTransform(const D3DXMATRIX &a_transform);

	inline const D3DXMATRIX & getPrevTransform() const { return prev_transform; }

private:
	void setCollisionPosition();
	void setCollisionTransform();

	void destroy( );
	void copy( const LocationCollisionable &source );
};

#endif
