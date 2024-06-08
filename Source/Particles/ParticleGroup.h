#ifndef PARTICLE_GROUP_H
#define PARTICLE_GROUP_H

#include <string>
#include <vector>
#include "CorePSubsystem.h"
#include "Util/Tracker/Tracker.h"

class IMesh;

struct CoreParticleGroup {
	std::string name;

	typedef std::vector<CorePSubsystem> CorePSubsystems;
	CorePSubsystems core_psubsystems;

	CoreParticleGroup( );
	~CoreParticleGroup( );
};

// -------------------------------------------------------------------------------

class ParticleGroup {
protected:
	int   particle_handle;
	const CoreParticleGroup *core;

	float life;
	bool  particle_group_finished;

	bool  fade_to_kill;
	float fade_alpha;
	float curr_time_fade;

	TTracker tracker;
	const IMesh *mesh;
	const Effect *shader;

public:
	ParticleGroup( );
	~ParticleGroup( );

	bool update( float deltaTime ); // Return true if it died
	void render();
	void kill( bool fading = false ); // Esto lo que hace es que en el update devuelva true

	inline void setHandleGroup( int aparticle_handle ) { particle_handle = aparticle_handle; }
	inline void setCoreParticleGroup( const CoreParticleGroup *acore ) { core = acore; }

	inline const TTracker &getTracker( ) const { return tracker; }
	inline void setTracker( const TTracker &atracker ) { tracker = atracker; }

	inline const CoreParticleGroup *getCore( ) const { return core; }

private:
	bool updateParticle( float deltaTime );
	inline int getFirstHandleGroup( ) {return particle_handle;}

	void renderSubsystem( const CorePSubsystem &subsystem );
	void setParticleDataToShader( size_t ini, size_t fin );
};

#endif
