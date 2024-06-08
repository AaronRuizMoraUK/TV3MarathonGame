#ifndef BOMBAS_H
#define BOMBAS_H

#include <vector>
#include "Object/Location.h"

class ParticleGroup;
class IntelligentObject;

class Rayo
{
public:
		enum Status {
		WARNING = 0
		, EXPLODING
		, BURNING
		, DEAD
	};

	TLocation loc;
	ParticleGroup *p;
	Status status;
	float currentTime;
	bool hasHit;

	// XML??

	float timeWarning;
	float timeExploding;
	float timeBurning;
	float radius_touch_player;
	float bomba_hit;

	float minRandomLeft;
	float maxRandomLeft;
	float minRandomFront;
	float maxRandomFront;

	Rayo(float aminRandomLeft, float amaxRandomLeft, float aminRandomFront, float amaxRandomFront);
	virtual ~Rayo();



	bool update (float deltaTime);
	void checkExploding();
	void explode();
	void isPlayerInsideResidual(float deltaTime);
	void checkPlayerEatsExplosion();
	void checkBurning();
};

class Bombas : public std::vector<Rayo *> {
public:
	~Bombas( );
	void clear( );

	void add(float aminRandomLeft, float amaxRandomLeft, float aminRandomFront, float amaxRandomFront);
	iterator del( iterator it );

	void update( float deltaTime );
};

#endif //BOMBAS_H
