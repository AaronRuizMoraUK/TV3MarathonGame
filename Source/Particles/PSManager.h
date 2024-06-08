#ifndef PSMANAGER_H
#define PSMANAGER_H

#include <string>
#include "Papi/pAPI.h"
#include "DomainGenerator.h"
#include "ActionPSManager.h"
#include "Expat/XMLParser.h"

struct CoreParticleGroup;
class ParticleGroup;
struct TTracker;

class PSManager : public CXMLParser {
public:
	// PAPI Context
	static PAPI::ParticleContext_t particleEngine;

protected:
	static std::string pathXML;

	// Core groups (XML)
	typedef std::map<std::string, CoreParticleGroup *> CoreParticleGroups;
	CoreParticleGroups core_particle_groups;

	// Current Particle Groups
	typedef std::vector<ParticleGroup *> ParticleGroups;
	ParticleGroups curr_particle_groups;

public:
	~PSManager();
	void destroy();
	
	static PSManager *get( );

	void load();
	void onStartElement( const std::string &elem, MKeyValue &atts );
	void onEndElement (const std::string &elem);
	void reload();

	// Añade una particula en el manager y él la maneja(update-"delete" y render)
	// Te la devuelve sólo por si quieres matarla antes de tiempo llamando a su kill()
	ParticleGroup *addParticleGroup(const std::string &core_name, const TTracker &tracker);

	// Crea una particula y la devuelve para que la manejes tu (update-"delete", render y kill)
	ParticleGroup *createParticleGroup(const std::string &core_name, const TTracker &tracker);

	void killAllParticleGroups( const std::string &core_name, bool fading = false );
	void killAll( bool fading = false );

	void update( float deltaTime );
	void render( );
	int printDebugInfo( int py = 0 );

	const std::string &getNameCore( size_t index ) const;
	size_t getNumCores( ) const { return core_particle_groups.size( ); }

private:
	static PSManager *ps_manager;
	PSManager();

	// Attributes used on parsing phase
	bool parsingIndex;
	std::queue< std::string > effectsFiles;
	
	bool openedActionList;
	DomainGenerator dGenerator;
	ActionPSManager aManager;
	
	std::string systemName;
	CoreParticleGroup *subsystemVector;

	// Methods used on parsing phase
	void loadEffects( );

	void onStartIndexElement (const std::string &elem, MKeyValue &atts);
	void onEndIndexElement (const std::string &elem);
	void onStartEffectElement (const std::string &elem, MKeyValue &atts);
	void onEndEffectElement (const std::string &elem);

	void parseSystemDefinition(MKeyValue &atts);
	void parseSubsystem(MKeyValue &atts);
	void parseTexture(MKeyValue &atts);
	void parseDomain(MKeyValue &atts);
	void parseBehaviour(MKeyValue &atts);

	void parseEndSystemDefinition();
	void parseEndSubsystem();
	void parseEndBehaviour();
};

#endif
