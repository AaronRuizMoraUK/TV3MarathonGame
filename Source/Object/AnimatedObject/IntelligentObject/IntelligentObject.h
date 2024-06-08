#ifndef INTELLIGENT_OBJECT_H
#define INTELLIGENT_OBJECT_H

#include "Object/AnimatedObject/CalAnimatedObject.h"

class IAIController;

/**
* This is an intelligent object. It is an CalAnimatedObject with an artificial
* intelligent controller
*/

class IntelligentObject : public CalAnimatedObject
{
public:
	static bool AI_enabled;

protected:
	IAIController *aiController;

public:
	IntelligentObject();
	IntelligentObject(const IntelligentObject &source);
	virtual ~IntelligentObject();

	void operator=(const IntelligentObject &source);

	bool updateAI(float deltaTime);

	IAIController * getAIController() const {return aiController;}
	void setAIController(IAIController *pAiController);

	void processSignal( TSignal tag, const void *data, size_t num_bytes );

	void collisionWith( const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal );

private:
	void destroy();
	void copy(const IntelligentObject &source);
};

#endif //INTELLIGENT_OBJECT_H
