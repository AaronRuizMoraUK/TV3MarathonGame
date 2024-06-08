#ifndef VALVE_CONTROLLER_H
#define VALVE_CONTROLLER_H

#include "AIControllers/IAIController.h"

class  ParticleGroup;
struct ROAResource;

class ValveController : public IAIController
{
public:
	enum Status {
		UNDEF = 0
		, LOW
		, MEDIUM
		, HIGH
		, NUM_STATUS
	};
	static const char *status_chars[ NUM_STATUS ];
	static Status getStatusFromText( const std::string &status_text );

protected:
	// XML
	Status      initial_status;
	Status      good_status;
	float       time_to_desynchronize;
	float       kick_radius;
	int         points_score_up;
	D3DXMATRIX  initial_transform;

	// Variables
	Status status;
	Status prev_status;
	float  time;

	ParticleGroup     *good_status_particles;
	IntelligentObject *int_obj;

	size_t num_valve;

private:
	const ROAResource *roa_resource;
	D3DXVECTOR3        initial_position;
	D3DXQUATERNION     initial_rotation;

public:
	ValveController( size_t anum_valve, Status ainitial_status, Status agood_status, float atime_to_desynchronize, float akick_radius, int apoints_score_up );
	~ValveController( );

	const char *getControllerType () const { return "ValveController"; }

	void reset( IntelligentObject &object );

	bool update(IntelligentObject &object, float deltaTime);

	inline Status getStatus( ) const { return status; }
	inline float getTimeToDesynchronize( ) const { return time_to_desynchronize; }
	inline float getTimeToExitGoodStatus( ) const { return time; }
	inline bool isOnRightStatus( ) const { return status == good_status; }

	void processSignal( TSignal tag, const void *data, size_t num_bytes );

	inline void setInitialTransform( const D3DXMATRIX &ainitial_transform ) { initial_transform = ainitial_transform; }

private:
	void nextStatus( );
	void changeTexture( IntelligentObject &object );
	bool hasPlayerKickValve( ) const;
};

#endif //VALVE_CONTROLLER_H
