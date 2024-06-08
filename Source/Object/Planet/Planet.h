#ifndef PLANET_H
#define PLANET_H

#include "Object/Object.h"
#include "PlanetMap.h"

/**
* A planet in scene. It is an Object with radius, name, heigthmap and normalmap.
*
* NOTA: Si se escala el planeta no funciona los planetmaps,
*       ya que las alturas y las normales cambian
*/

class Planet : public Object
{
public:
	enum SurfaceType {
		UNDEF = 0
		, DAMAGE
		, SAFE
	};

protected:
	std::string name;
	float backside_frustum_radius;

	const PlanetMap<float>       *heightMap;	// Pertenece a PlanetMapManager
	const PlanetMap<D3DXVECTOR3> *normalMap;	// Pertenece a PlanetMapManager
	const PlanetMap<int>         *textureMap;	// Pertenece a PlanetMapManager

public:
	Planet();
	~Planet();

	inline const D3DXVECTOR3 & getCenter() const {return loc.getPosition();}
	inline void setCenter(const D3DXVECTOR3 &pCenter) { loc.setPosition(pCenter); }

	inline const std::string & getName() const {return name;}
	inline const float getBackSideFrustumRadius( ) const { return backside_frustum_radius; }

	inline void setName(const std::string &pName) { name=pName; }
	inline void setBackSideFrustumRadius( float abackside_frustum_radius ) { backside_frustum_radius = abackside_frustum_radius; }
	inline void setHeightMap( const PlanetMap<float> *aheightMap )       { heightMap = aheightMap; }
	inline void setNormalMap( const PlanetMap<D3DXVECTOR3> *anormalMap ) { normalMap = anormalMap; }
	inline void setTextureMap( const PlanetMap<int> *atextureMap )        { textureMap = atextureMap; }
	
	void getRandomPolarAngles( float &alphaOut, float &deltaOut ) const;
	void getPolarAngles(const D3DXVECTOR3 &globalPoint, float &alphaOut, float &deltaOut);
	void getPointOnPlanet( D3DXVECTOR3 &output, float alpha, float delta ) const;

	float getHeight(float alpha, float delta) const;
	void getNormal(D3DXVECTOR3 &output, float alpha, float delta) const;
	SurfaceType getSurfaceType( float alpha, float delta ) const; 
	
	// Esto se utiliza para inicializar un objecto en algun punto del planeta
	// NOTE: El left se va a calcular con el up(0,1,0)
	void getTransformOnPlanet( D3DXMATRIX &out_trans, float local_yaw, float alpha, float delta ) const; 
	void getTransformOnPlanetUsingCenter( D3DXMATRIX &out_trans, float local_yaw, float alpha, float delta ) const; 

private:
	void getNormalInLocal(D3DXVECTOR3 &output, float alpha, float delta) const;
};

#endif //PLANET_H
