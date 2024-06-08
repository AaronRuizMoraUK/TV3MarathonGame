#include <cassert>
#include "Planet.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"

Planet::Planet()
: name("")
, backside_frustum_radius( 20.0f )
, heightMap( NULL )
, normalMap( NULL )
, textureMap( NULL )
{
}

Planet::~Planet() {
	printDebugString("Destroying Planet...\n");
}

void Planet::getRandomPolarAngles( float &alphaOut, float &deltaOut ) const {
	// El limit es para que no salgan angulos límite
	static const float limit = 0.1f;
	alphaOut = obtainRandom( limit, 2.0f*D3DX_PI - limit );
	deltaOut = obtainRandom( -D3DX_PI/2.0f + limit, D3DX_PI/2.0f - limit );
}

void Planet::getPolarAngles(const D3DXVECTOR3 &point, float &alphaOut, float &deltaOut) {
	// Obtain the direction vector
	D3DXVECTOR3 direction = point - getCenter( );
	normalizeVector3(direction, direction);

	// Obtain in Planet Local Coordinates from Global Coordinates of the direction
	D3DXVec3TransformNormal(&direction, &direction, &loc.getInverseTransform( ));

	// Set alpha and delta angles from local point
	vectorToAngles( direction, alphaOut, deltaOut );
	deltaOut = -deltaOut;
	if(alphaOut<0.0f)
		alphaOut += 2.0f*D3DX_PI;

}

void Planet::getPointOnPlanet( D3DXVECTOR3 &output, float alpha, float delta ) const {
	D3DXVECTOR3 direction;
	anglesToVector( alpha, delta, direction );
	direction.y = -direction.y;
	D3DXVec3TransformNormal(&direction, &direction, &loc.getTransform( )); // Obtain direction in Global Coordinates

	float height = getHeight( alpha, delta );
	output = getCenter( ) + height * direction;
}

float Planet::getHeight(float alpha, float delta) const {
	if( !heightMap )
		return 0.0f;
	else
		return heightMap->getData( alpha, delta );
}

void Planet::getNormalInLocal(D3DXVECTOR3 &output, float alpha, float delta) const {
	output = ( !normalMap ) ? D3DXVECTOR3( 0,0,1 ) : normalMap->getData( alpha, delta );
}

void Planet::getNormal(D3DXVECTOR3 &output, float alpha, float delta) const {
	getNormalInLocal( output, alpha, delta );
	D3DXVec3TransformNormal( &output, &output, &loc.getTransform( ) ); // Obtain direction in Global Coordinates
}

Planet::SurfaceType Planet::getSurfaceType( float alpha, float delta ) const {
	if( !textureMap )
		return UNDEF;
	else {
		int surface_type = textureMap->getData( alpha, delta );
		if( surface_type == 1 )
			return DAMAGE;
		else if( surface_type == 2 )
			return SAFE;
		else {
			assert( !"Texture map incorrect!!" );
			return SAFE;
		}
	}
}

void Planet::getTransformOnPlanet( D3DXMATRIX &output, float local_yaw, float alpha, float delta ) const {
	assert( local_yaw>=0.0f && local_yaw<2.0f*D3DX_PI );
	static const D3DXVECTOR3 up(0,1,0);

	D3DXVECTOR3 position_on_planet;
	getPointOnPlanet( position_on_planet, alpha, delta );

	D3DXVECTOR3 local_up;
	getNormal( local_up, alpha, delta );
	normalizeVector3( local_up, local_up );
	
	D3DXVECTOR3 local_left;
	D3DXVECTOR3 local_front;
	if( local_up == up ) {
		local_left = D3DXVECTOR3( 1,0,0 );
		local_front = D3DXVECTOR3( 0,0,1 );
	}
	else {
		D3DXVec3Cross( &local_left, &local_up, &up );
		normalizeVector3( local_left, local_left );
		D3DXVec3Cross( &local_front, &local_left, &local_up );
	}

	D3DXMatrixIdentity( &output );
	output(0,0) = local_left.x;
	output(0,1) = local_left.y;
	output(0,2) = local_left.z;
	output(1,0) = local_up.x;
	output(1,1) = local_up.y;
	output(1,2) = local_up.z;
	output(2,0) = local_front.x;
	output(2,1) = local_front.y;
	output(2,2) = local_front.z;
	output(3,0) = position_on_planet.x;
	output(3,1) = position_on_planet.y;
	output(3,2) = position_on_planet.z;

	if( local_yaw != 0.0f )
		rotateOnAxis( output, output, local_up, local_yaw );
}

void Planet::getTransformOnPlanetUsingCenter( D3DXMATRIX &output, float local_yaw, float alpha, float delta ) const {
	assert( local_yaw>=0.0f && local_yaw<2.0f*D3DX_PI );
	static const D3DXVECTOR3 up(0,1,0);

	D3DXVECTOR3 position_on_planet;
	getPointOnPlanet( position_on_planet, alpha, delta );

	D3DXVECTOR3 local_up = position_on_planet - getCenter( );
	normalizeVector3( local_up, local_up );
	
	D3DXVECTOR3 local_left;
	D3DXVECTOR3 local_front;
	if( local_up == up ) {
		local_left = D3DXVECTOR3( 1,0,0 );
		local_front = D3DXVECTOR3( 0,0,1 );
	}
	else {
		D3DXVec3Cross( &local_left, &local_up, &up );
		normalizeVector3( local_left, local_left );
		D3DXVec3Cross( &local_front, &local_left, &local_up );
	}

	D3DXMatrixIdentity( &output );
	output(0,0) = local_left.x;
	output(0,1) = local_left.y;
	output(0,2) = local_left.z;
	output(1,0) = local_up.x;
	output(1,1) = local_up.y;
	output(1,2) = local_up.z;
	output(2,0) = local_front.x;
	output(2,1) = local_front.y;
	output(2,2) = local_front.z;
	output(3,0) = position_on_planet.x;
	output(3,1) = position_on_planet.y;
	output(3,2) = position_on_planet.z;

	if( local_yaw != 0.0f )
		rotateOnAxis( output, output, local_up, local_yaw );
}
