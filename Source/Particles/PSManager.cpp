#include "PSManager.h"
#include "ParticleGroup.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Factories/TextureFactory.h"
#include "Util/Bounding/Bounding.h"

std::string PSManager::pathXML = "";
PSManager *PSManager::ps_manager = NULL;

const std::string MODULATE = "modulate_particle_tech";
const std::string ADDITIVE = "add_particle_tech";

PAPI::ParticleContext_t PSManager::particleEngine;

PSManager::PSManager()
: parsingIndex( false )
, openedActionList( false )
, systemName( "" )
, subsystemVector( NULL )
{
}

PSManager::~PSManager() {
	printDebugString( "Destroying PSManager...\n" );
	destroy( );
}

void PSManager::destroy() {
	// Destroy particles instances
	ParticleGroups::iterator itPG = curr_particle_groups.begin( );
	while( itPG != curr_particle_groups.end( ) ) {
		delete (*itPG), (*itPG) = NULL;
		++itPG;
	}
	curr_particle_groups.clear( );

	// Destroy cores
	CoreParticleGroups::iterator itC = core_particle_groups.begin( );
	while( itC != core_particle_groups.end( ) ) {
		delete itC->second, itC->second = NULL;
		++itC;
	}
	core_particle_groups.clear( );

	// Release PAPI
	// No necesario
}

PSManager * PSManager::get() {
	if(ps_manager == NULL)
		ps_manager = new PSManager();

	return ps_manager;
}

void PSManager::load() {
	// Load PAPI
	// No necesario

	// Read Core Particles Groups from XML
	if( pathXML.empty( ) )
		pathXML = Global::data_locator.getPath( "particles" );

	// Load all particles
	parsingIndex = true;
	bool ok = xmlParseFile( pathXML + "particles_index.xml" );
	assert( ok );

	// Now load effects
	loadEffects( );
}

void PSManager::loadEffects( ) {
	parsingIndex = false;

	while (!effectsFiles.empty()) {
		const std::string &filename = effectsFiles.front();

		bool ok = xmlParseFile( pathXML + filename );
		assert( ok );

		effectsFiles.pop();
	}
}

void PSManager::reload() {
	destroy( );
	load( );
}

void PSManager::onStartElement( const std::string &elem, MKeyValue &atts ) {
	if (parsingIndex)
		onStartIndexElement(elem, atts);
	else
		onStartEffectElement(elem, atts);
}

void PSManager::onEndElement (const std::string &elem) {
	if (parsingIndex)
		onEndIndexElement(elem);
	else
		onEndEffectElement(elem);
}

void PSManager::onStartIndexElement (const std::string &elem, MKeyValue &atts) {
	if (elem == "particle_group")
		effectsFiles.push( atts["filename"] );
}

void PSManager::onEndIndexElement (const std::string &elem) {
}

void PSManager::onStartEffectElement (const std::string &elem, MKeyValue &atts) {
	if (elem == "SYSTEM_DEFINITION") {
		parseSystemDefinition(atts);
	} else {
		if (elem == "SUBSYSTEM") {
			parseSubsystem(atts);
		} else {
			if (elem == "BEHAVIOUR") {
				parseBehaviour(atts);
			} else {
				if (elem == "TEXTURE") {
					parseTexture(atts);
				} else {
					if (elem == "DOMAIN") {
						if (!openedActionList)
							parseBehaviour(atts);
						parseDomain(atts);
					} else {
						if (elem == "FUNCTION") {	
							if (!openedActionList)
								parseBehaviour(atts);
							aManager.addNewFunction();
						} else {
							if (elem == "SAMPLE") {		
								if (!openedActionList)
									parseBehaviour(atts);
								aManager.addSample(atts);
							} else {
								if (!openedActionList)
									parseBehaviour(atts);
								aManager.enterAction(elem, atts);
							}
						}
					}
				}
			}
		}
	}
}

void PSManager::onEndEffectElement (const std::string &elem) {
	if (elem == "SYSTEM_DEFINITION") {
		parseEndSystemDefinition();
	} else {
		if (elem == "SUBSYSTEM") {
			parseEndSubsystem();
		} else {
			if (elem == "BEHAVIOUR") {
				parseEndSubsystem();
			} else {
				aManager.exitAction(elem, dGenerator, particleEngine);
			}
		}
	}
}

void PSManager::parseSystemDefinition(MKeyValue &atts) {
	systemName = atts["name"];
	subsystemVector = NULL;
	subsystemVector = new CoreParticleGroup( );
	subsystemVector->name = systemName;
}

void PSManager::parseSubsystem(MKeyValue &atts) {
	CorePSubsystem subsystem;

	subsystem.setMaxParticles(atts.getInt("max", 1));
	std::string shaderType = atts.getString("effect", "ADD");
	if (shaderType == "MODULATE") {
		subsystem.setShaderType(MODULATE);
	} else {
		subsystem.setShaderType(ADDITIVE);
	}
	subsystem.setParticleColorFactor((atts.getFloat("particleColorFactor", 0.0f)));

	subsystemVector->core_psubsystems.push_back(subsystem);
}

void PSManager::parseTexture(MKeyValue &atts) {
	const size_t indexSubsystem = subsystemVector->core_psubsystems.size() - 1;
	assert(indexSubsystem >= 0);

	const Texture *texture = TextureFactory::get( )->createTexture( atts["diffuse"] );
	assert( texture );

	subsystemVector->core_psubsystems[indexSubsystem].setTexture( texture );
}

void PSManager::parseDomain(MKeyValue &atts) {
	dGenerator.pushDomain(atts);
}

void PSManager::parseBehaviour(MKeyValue &atts) {
	openedActionList = true;
	int handle = particleEngine.GenActionLists();

	const size_t indexSubsystem = subsystemVector->core_psubsystems.size() - 1;
	subsystemVector->core_psubsystems[indexSubsystem].setUpdateHandle(handle, atts.getFloat("initTime", 0.0f), atts.getFloat("finishTime", -1.0f)); 

	// Starting new Action List
	particleEngine.NewActionList(handle);
}

void PSManager::parseEndSystemDefinition() {
	CoreParticleGroups::iterator it = core_particle_groups.find(systemName);
	if (it != core_particle_groups.end()) {
		// Removing previous definition
		CoreParticleGroup *aux = it->second;
		delete aux;
	}
	// Adding new definition
	core_particle_groups[systemName] = subsystemVector;
	dGenerator.clear();
}

void PSManager::parseEndSubsystem() {
	if (openedActionList) {
		particleEngine.EndActionList();
		openedActionList = false;
	}
}

void PSManager::parseEndBehaviour() {
	if (openedActionList) {
		particleEngine.EndActionList();
		openedActionList = false;
	}
}

// --------------------------------------------------------------------------------------

ParticleGroup *PSManager::addParticleGroup(const std::string &core_name, const TTracker &tracker) {
	ParticleGroup *new_particle_group = createParticleGroup( core_name, tracker );

	// Register in my list of particle groups so PSManager can manage it
	if( new_particle_group != NULL )
		curr_particle_groups.push_back( new_particle_group );

	return new_particle_group;
}

ParticleGroup *PSManager::createParticleGroup(const std::string &core_name, const TTracker &tracker) {
	ParticleGroup *new_particle_group = NULL;

	CoreParticleGroups::iterator it = core_particle_groups.find(core_name);
	if(it != core_particle_groups.end()) {
		CoreParticleGroup *core_particle_group = it->second;
		assert( core_particle_group );

		int num_subsys = (int)core_particle_group->core_psubsystems.size();
		int particle_group_handle = particleEngine.GenParticleGroups(num_subsys, 1);
		
		for (int i=0; i<num_subsys; ++i) {
			particleEngine.CurrentGroup(i + particle_group_handle);
			particleEngine.SetMaxParticles( core_particle_group->core_psubsystems[i].getMaxParticles() );
		}

		new_particle_group = new ParticleGroup();
		assert( new_particle_group );
		new_particle_group->setHandleGroup( particle_group_handle );
		new_particle_group->setCoreParticleGroup( it->second );
		new_particle_group->setTracker( tracker );
	}
	else {
		warn( "Particle \"%s\" does not exist", core_name.c_str( ) );
	}

	return new_particle_group;
}

void PSManager::killAllParticleGroups( const std::string &core_name, bool fading ) {
	ParticleGroups::iterator it = curr_particle_groups.begin( );
	while( it != curr_particle_groups.end( ) ) {
		ParticleGroup *pgroup = *it;
		assert( pgroup );
		assert( pgroup->getCore( ) );

		if( pgroup->getCore( )->name == core_name )
			pgroup->kill( fading );

		++it;
	}
}

void PSManager::killAll( bool fading ) {
	ParticleGroups::iterator it = curr_particle_groups.begin( );
	while( it != curr_particle_groups.end( ) ) {
		ParticleGroup *pgroup = *it;
		assert( pgroup );

		pgroup->kill( fading );

		++it;
	}
}

// --------------------------------------------------------------------------------------

void PSManager::update( float deltaTime ) {
	ParticleGroups::iterator it = curr_particle_groups.begin( );
	while( it != curr_particle_groups.end( ) ) {
		ParticleGroup *pgroup = *it;
		assert( pgroup );

		// Aqui es el unico lugar donde se destruyen las particulas del manager
		bool died = pgroup->update( deltaTime );
		if( died ) {
			delete pgroup, pgroup = NULL;
			it = curr_particle_groups.erase( it );
			continue;
		}
		else
			++it;
	}
}

void PSManager::render( ) {
	ParticleGroups::iterator it = curr_particle_groups.begin( );
	while( it != curr_particle_groups.end( ) ) {
		ParticleGroup *pgroup = *it;
		assert( pgroup );

		BoundingSphere bsphere( D3DXVECTOR3(0,1,0), 1.0f );
		bsphere.center += pgroup->getTracker( ).getPosition( );
		if( Global::frustum.isBoundingVisible( bsphere ) && Global::back_side_frustum.isBoundingVisible( bsphere ) )
			pgroup->render( );

		++it;
	}
}


int PSManager::printDebugInfo( int py ) {
#ifndef NDEBUG
	py += printf( 0, py, 0xffff0000, "Num Particle groups %d", curr_particle_groups.size( ) );

	ParticleGroups::iterator it = curr_particle_groups.begin( );
	while( it != curr_particle_groups.end( ) ) {
		ParticleGroup *pgroup = *it;
		assert( pgroup );

		py += printf( 0, py, 0xffff0000, "Particle Group: \"%s\"", pgroup->getCore( )->name.c_str( ) );

		++it;
	}
#endif

	return py;
}

const std::string &PSManager::getNameCore( size_t index ) const {
	assert( index < core_particle_groups.size( ) ); 

	CoreParticleGroups::const_iterator it = core_particle_groups.begin( );
	for( size_t i=0; i<index && it != core_particle_groups.end( ); ++i )
		++it;
	assert( it->second );
	return it->second->name;
}
