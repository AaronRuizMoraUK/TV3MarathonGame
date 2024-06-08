#ifndef POWER_UP_CONTROLLER_H
#define POWER_UP_CONTROLLER_H

#include "AIControllers/IAIController.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"

class PowerUpController : public IAIController
{
public:
	enum Type {
		LIFE = 0
		, SCORE
		, NUM_TYPES
	};
	static const char *types_chars[ NUM_TYPES ];
	static Type getTypeFromText( const std::string &type_text );

protected:
	// XML
	Type  type;
	float  points;
	size_t score_for_taking;
	float  radius;

	// Variables

public:
	PowerUpController( Type atype, float apoints, size_t ascore_for_taking, float aradius );
	~PowerUpController( );

	const char *getControllerType () const { return "PowerUpController"; }

	bool update(IntelligentObject &object, float deltaTime);

private:
	bool hasPlayerTakePowerUp( const IntelligentObject &object ) const;
	void applyPowerUp( );
};

#endif //POWER_UP_CONTROLLER_H
