#include "CalAnimModel.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

std::string CalAnimCoreModel::pathToCal = "";

CalAnimCoreModel::CalAnimCoreModel( const std::string &aname )
: name( aname )
, calCoreModel(aname)
, meshId( -1 )
, reload_time_factors( false )
{
}

CalAnimCoreModel::~CalAnimCoreModel( ) {
	printDebugString("Destroying CalAnimCoreModel...\n");
}

void CalAnimCoreModel::onStartElement(const std::string &elem, MKeyValue &atts) {
	if( reload_time_factors ) {
		if (elem=="animation") {
			CoreAnim &core_anim = getCoreAnim( atts["name"] );
			core_anim.def_delay_in    = atts.getFloat( "def_delay_in", 0.0f );
			core_anim.def_delay_out   = atts.getFloat( "def_delay_out", 1.0f );
			core_anim.def_time_factor = atts.getFloat( "def_time_factor", 1.0f );
			assert( core_anim.def_delay_in   >= 0.0f );
			assert( core_anim.def_delay_out  >= 0.0f );
			assert( core_anim.def_time_factor > 0.0f );
		}
	}
	else {
		if( pathToCal.empty( ) )
			pathToCal = Global::data_locator.getPath( "cal" );

		if (elem=="skeleton") {
			std::string pathFilename = pathToCal + name + "/" + atts["filename"];
			bool ok = calCoreModel.loadCoreSkeleton( pathFilename );
			assert(ok);
		}
		else if (elem=="animation") {
			const std::string &anim_name = atts["name"];
			std::string pathFilename = pathToCal + name + "/" + atts["filename"];

			CoreAnim core_anim;
			core_anim.id = calCoreModel.loadCoreAnimation( pathFilename, anim_name );
			assert( core_anim.id>=0 );

			core_anim.def_delay_in    = atts.getFloat( "def_delay_in", 0.0f );
			core_anim.def_delay_out   = atts.getFloat( "def_delay_out", 1.0f );
			core_anim.def_time_factor = atts.getFloat( "def_time_factor", 1.0f );
			assert( core_anim.def_delay_in   >= 0.0f );
			assert( core_anim.def_delay_out  >= 0.0f );
			assert( core_anim.def_time_factor > 0.0f );

			core_anim.cal_core_animation = calCoreModel.getCoreAnimation( core_anim.id );
			assert( core_anim.cal_core_animation );

			coreAnimMap[ anim_name ] = core_anim;
		}
		else if (elem=="mesh") {
			std::string pathFilename = pathToCal + name + "/" + atts["filename"];
			meshId = calCoreModel.loadCoreMesh( pathFilename );
			assert( meshId >= 0 );
		}
	}
}

CalAnimCoreModel::CoreAnim zero;
const CalAnimCoreModel::CoreAnim &CalAnimCoreModel::getCoreAnim( const std::string &anim_name ) const {
	CoreAnimMap::const_iterator it = coreAnimMap.find( anim_name );
	if( it != coreAnimMap.end() )
		return it->second;
	else {
		assert( !"CoreAnim not found" );
		return zero;
	}
}

CalAnimCoreModel::CoreAnim &CalAnimCoreModel::getCoreAnim( const std::string &anim_name ) {
	CoreAnimMap::iterator it = coreAnimMap.find( anim_name );
	if( it != coreAnimMap.end() )
		return it->second;
	else {
		assert( !"CoreAnim not found" );
		return zero;
	}
}

void CalAnimCoreModel::reloadTimeFactors( ) {
	reload_time_factors = true;

	std::string pathFilename = pathToCal + name + "/cal_core_model.xml";
	bool ok = xmlParseFile( pathFilename );
	assert( ok );

	reload_time_factors = false;
}

/*
// NO FUNCIONA BIEN
void CalAnimCoreModel::reload( ) {
int ok;
// No se puede recargar el esqueleto

// Recarga de animaciones
IdAnimMap::iterator it = idAnimMap.begin( );
while( it != idAnimMap.end( ) ) {
int &idAnim = it->second;

CalCoreAnimation *core_anim = calCoreModel.getCoreAnimation( idAnim );
assert( core_anim );
std::string anim_filename = core_anim->getFilename( ); // Copiar filename, ya que vamos a recargarlo

ok = calCoreModel.unloadCoreAnimation( idAnim );  // Descargar
assert( ok != -1 );
idAnim = calCoreModel.loadCoreAnimation( anim_filename ); // Cargar y guarar el idAnim
assert( idAnim >= 0 );

++it;
}

// Recarga de mesh
CalCoreMesh *core_mesh = calCoreModel.getCoreMesh( meshId );
assert( core_mesh );
std::string mesh_filename = core_mesh->getFilename( ); // Copiar filename, ya que vamos a recargarlo
ok = calCoreModel.unloadCoreMesh( meshId );
assert( ok != -1 );
meshId = calCoreModel.loadCoreMesh( mesh_filename );
assert( meshId >= 0 );
}
*/

// -----------------------------------------------------------------

CalAnimModel::CalAnimModel( const CalAnimCoreModel *acore )
: calModel( (CalCoreModel *)&acore->calCoreModel ) // El cast es por culpa de cal3d
, core( acore )
{
}

CalAnimModel::CalAnimModel( const CalAnimModel &source )
: calModel( (CalCoreModel *)&source.core->calCoreModel ) // El cast es por culpa de cal3d
, core( source.core )
{
}

CalAnimModel::~CalAnimModel() {
	printDebugString("Destroying CalAnimModel...\n");
}

void CalAnimModel::operator=( const CalAnimModel &source ) {
	assert( !"Operator = not allowed in CalAnimModel" );
}

void CalAnimModel::playCycle(const std::string &name, float delay) {
	if( !CalAnimatedObject::cal_animation_enabled )
		return;

	assert( core );
	const CalAnimCoreModel::CoreAnim &core_anim = core->getCoreAnim( name );
	float delay_anim = (delay!=-1.0f) ? delay : core_anim.def_delay_in;

	clearAllCycles( delay_anim );
	blendCycle( core_anim.id, core_anim.def_time_factor, delay_anim );
}

float CalAnimModel::playAction(const std::string &name, bool autoLock, float delayIn, float delayOut) {
	if( !CalAnimatedObject::cal_animation_enabled )
		return 0.0f;

	assert( core );
	const CalAnimCoreModel::CoreAnim &core_anim = core->getCoreAnim( name );

	calModel.getMixer( )->executeAction( core_anim.id, core_anim.def_time_factor
		, (delayIn!=-1.0f) ? delayIn : core_anim.def_delay_in
		, (delayOut!=-1.0f) ? delayOut : core_anim.def_delay_out
		, 1.0f, autoLock );

	assert( core_anim.cal_core_animation );
	assert( core_anim.def_time_factor != 0.0f );
	return core_anim.cal_core_animation->getDuration( ) / core_anim.def_time_factor;
}

void CalAnimModel::removeAction(const std::string &name, float delay) {
	assert( core );
	const CalAnimCoreModel::CoreAnim &core_anim = core->getCoreAnim( name );

	CalAnimationAction *action = getActionInMixer( core_anim.cal_core_animation );
	if( action==NULL || action->hasBeenCancelled( ) )
		return;

	action->remove( (delay!=-1.0f) ? delay : core_anim.def_delay_out );
}

void CalAnimModel::clearAllCycles( float delay ) {
	CalMixer *mixer = calModel.getMixer( );
	assert( mixer );

	// Remove all cycles
	typedef std::list<CalAnimationCycle *> LCycles;
	LCycles & cycles = mixer->getAnimationCycle();
	LCycles::iterator itC = cycles.begin();
	while( itC != cycles.end() ) {
		CalAnimationCycle *cycle = *itC;
		assert( cycle );
		assert( cycle->getCoreAnimation() );

		const std::string &anim_name = cycle->getCoreAnimation()->getName( );
		clearCycle( anim_name, delay );
		++itC;
	}

	if( delay == 0.0f )
		calModel.update( 0.0f );
}

void CalAnimModel::clearAllActions( float delay ) {
	CalMixer *mixer = calModel.getMixer( );
	assert( mixer );

	// Remove all actions
	typedef std::list<CalAnimationAction *> LActions;
	LActions &actions = mixer->getAnimationActionList( );
	LActions::iterator itA = actions.begin( );
	while( itA != actions.end( ) ) {
		assert( (*itA) );
		(*itA)->remove( delay );
		++itA;
	}

	if( delay == 0.0f )
		calModel.update( 0.0f );
}

void CalAnimModel::clearAllAnimations( float delay ) {
	CalMixer *mixer = calModel.getMixer( );
	assert( mixer );

	// Remove all actions
	typedef std::list<CalAnimationAction *> LActions;
	LActions &actions = mixer->getAnimationActionList( );
	LActions::iterator itA = actions.begin( );
	while( itA != actions.end( ) ) {
		assert( (*itA) );
		(*itA)->remove( delay );
		++itA;
	}

	// Remove all cycles
	typedef std::list<CalAnimationCycle *> LCycles;
	LCycles & cycles = mixer->getAnimationCycle();
	LCycles::iterator itC = cycles.begin();
	while( itC != cycles.end() ) {
		CalAnimationCycle *cycle = *itC;
		assert( cycle );
		assert( cycle->getCoreAnimation() );

		const std::string &anim_name = cycle->getCoreAnimation()->getName( );
		clearCycle( anim_name, delay );
		++itC;
	}

	if( delay == 0.0f )
		calModel.update( 0.0f );
}

void CalAnimModel::blendCycle(const std::string &name, float delay) {
	assert( core );
	const CalAnimCoreModel::CoreAnim &core_anim = core->getCoreAnim( name );

	blendCycle( core_anim.id, core_anim.def_time_factor
		, (delay!=-1.0f) ? delay : core_anim.def_delay_in );
}

void CalAnimModel::clearCycle(const std::string &name, float delay) {
	assert( core );
	const CalAnimCoreModel::CoreAnim &core_anim = core->getCoreAnim( name );

	calModel.getMixer( )->clearCycle( core_anim.id
		, (delay!=-1.0f) ? delay : core_anim.def_delay_out );
}

void CalAnimModel::blendCycle(int id, float time_factor, float delay) {
	// No termina de estar bien en los tiempos de blending
	//calModel.getMixer( )->setTimeFactor( time_factor );

	calModel.getMixer( )->blendCycle( id, time_factor, 1.0f, delay );
}

CalAnimationAction * CalAnimModel::getActionInMixer( const CalCoreAnimation *cal_core_animation ) {
	if( !cal_core_animation )
		return NULL;

	typedef std::list<CalAnimationAction *> CalActions;
	CalActions &cal_actions = calModel.getMixer( )->getAnimationActionList( );
	CalActions::iterator it = cal_actions.begin( );
	while( it != cal_actions.end( ) ) {
		assert( (*it) );
		// Si tienen el mismo core animation
		if( (*it)->getCoreAnimation( ) == cal_core_animation )
			return (*it);
		++it;
	}

	return NULL;
}
