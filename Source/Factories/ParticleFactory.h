#ifndef PARTICLE_FACTORY_H
#define PARTICLE_FACTORY_H

#include <map>

class IMesh;

/**
* Factory of particle mesh.
*
* This is a singleton.
*/

class ParticleFactory
{
protected:
	typedef std::map<int, const IMesh *> ParticleMeshMap;
	ParticleMeshMap particleMeshMap;

public:
	~ParticleFactory();

	static ParticleFactory * get( );

	const IMesh * createParticle(int numParticles);

private:
	static ParticleFactory * particleFactory;
	ParticleFactory() {}

	const IMesh * createParticleMesh(int numParticles);
};

#endif //PARTICLE_FACTORY_H
