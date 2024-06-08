#ifndef OBJECT_H
#define OBJECT_H

#include "Location.h"
#include "Material/Material.h"
#include "Util/Signal/Signal.h"
#include "Managers/CollisionManager.h"

class IMesh;
class ParticleGroup;

/**
* Object in scene. This has position, transform matrix, mesh and
* material vector.
*/

class Object : public SignalListener
{
public:
	LocationCollisionable loc;

protected:
	const IMesh *mesh; // Pertenece a MeshFactory
	ParticleGroup *particle_group; // Es una particula que siempre vaya a estar junto al objeto. No se copia.

	MaterialVector materialVector;

public:
	Object();
	Object(const Object &source);
	virtual ~Object();
	
	virtual void operator=(const Object &source);

	virtual void render();
	virtual void renderDebug( );

	inline const IMesh * getMesh() const {	return mesh; }
	inline void setMesh(const IMesh *pMesh) { mesh=pMesh; }

	inline void setParticleGroup(ParticleGroup *aparticle_group) { particle_group=aparticle_group; }

	void SETMATERIAL (const std::string &texture, const std::string &shader, const std::string &technique);
	void SETMATERIAL (const Texture *texture, const std::string &shader, const std::string &technique);
	void CHANGETEXTURE( const std::string &texture );
	void CHANGETEXTUREANDNORMAL(const std::string &texture, const std::string &normalmap_texture);

	Material *getMaterial( );

	virtual void collisionWith( const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal ) {}
	
private:
	void destroy();
	void copy(const Object &source);
};

#endif //OBJECT_H
