#ifndef EXPENDER_CONTROLLER_H
#define EXPENDER_CONTROLLER_H

#include "AIControllers/IAIController.h"

class  ParticleGroup;
struct ROAResource;

class ExpenderController : public IAIController
{
protected:
	// XML
	std::string enemy_type;  // Tipo de enemigo que nace al pegar una patada
	int         num_enemies_can_expel;
	float       max_recover_time;
	float       kick_radius;
	int         points_score_up;
	D3DXMATRIX  initial_transform;

	// Variables
	int    enemies_counter;
	float  recover_time;
	bool   overheated;

	ParticleGroup     *overheated_particles;
	IntelligentObject *int_obj;

private:
	const ROAResource *roa_resource;
	D3DXVECTOR3        initial_position;
	D3DXQUATERNION     initial_rotation;

public:
	ExpenderController( const std::string &aenemy_type, int anum_enemies_can_expel, float amax_recover_time, float akick_radius, int apoints_score_up );
	~ExpenderController( );

	const char *getControllerType () const { return "ExpenderController"; }

	void reset( IntelligentObject &object );

	bool update(IntelligentObject &object, float deltaTime);

	void processSignal( TSignal tag, const void *data, size_t num_bytes );

	inline void setInitialTransform( const D3DXMATRIX &ainitial_transform ) { initial_transform = ainitial_transform; }

private:
	void expenderKicked( );
	void changeTexture( IntelligentObject &object );
	bool hasPlayerKickValve( ) const;
};

#endif //EXPENDER_CONTROLLER_H
