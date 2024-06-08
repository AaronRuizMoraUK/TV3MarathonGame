#include "SecondLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Bass/SoundPlayer.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Managers/EnemyManager.h"
#include "Util/Tracker/Tracker.h"
#include "Camera/ICamera.h"
#include "Camera/Camera3D.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "Particles/ParticleGroup.h"
#include "AIControllers/Follower/Follower.h"
#include "PlayerControllers/SpherePlayerController/SpherePlayerController.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "GameState/IGameState.h"

SecondLevelLogic::SecondLevelLogic()
: must_show_welcome_image( true )
, show_dh_first_time_gets_bomb (true)
, show_dh_first_time_explodes_planted(true)
, show_dh_first_time_show_planet_gameplay(true)

// Variables que necesitan resetearse
, timer_rayos(0.0f)
, particles(NULL)
, serotoninaRemaining( 0 )
, noradrenalinaRemaining( 0 )
, player_win( false )
, alphaRayo( 0.0f )
, deltaRayo( 0.0f )
, state(1)
, previousState(0)
, current_syn_player_lose_per_second(0.0f)
, time_to_born_powerup( 0.0f )
, currentTipo(-1)
, bomba(NULL)
, mustDeleteBomba(false)
, idSonido(0)
, new_tiempo_rayos(2.0f)

// XML. Cargadas en el Load, no hay que resetearlas
, init_player_sync( 250.0f )
, syn_player_lose_per_second( 0.5f ) // TODO ESTA VARIABLE NO DEBE CAMBIAR, YA QUE ES DEL XML
, tiempo_rayos( 20.0f )
, radius_zona_plantar(0.5f)
, alpha_nacimiento(0.0f)
, delta_nacimiento(0.0f)
, time_to_show_planet_gameplay_msg( 1.0f )
, minRandomFront(0.0f)
, maxRandomFront(0.0f)
, minRandomLeft(0.0f)
, maxRandomLeft(0.0f)
, max_time_to_born_powerup( 1.0f )
{

}

void SecondLevelLogic::destroyLevel( ) {
	// Destroy Triggers, Spatial and particles
	Global::triggers->clear( );
	Global::spatial.clear( );
	PSManager::get( )->killAll( );

	// Destroy Second Level Things
	power_ups.clear( );

	// Reset Variables
	timer_rayos = 0.0f;
	particles = NULL;
	serotoninaRemaining = 0;
	noradrenalinaRemaining = 0;
	player_win = false;
	alphaRayo = 0.0f;
	deltaRayo = 0.0f;
	state = 1;
	mustDeleteBomba = false;
	currentTipo = -1;
	idSonido = 0;
	new_tiempo_rayos = obtainRandom(2.0f, 3.0f);
	time_to_born_powerup = 0.0f;

	if (bomba)
	{
		Global::spatial.removeObject( bomba ); // Remove it to the spatial
		delete bomba, bomba = NULL;
	}

	rayos.clear();
}

void SecondLevelLogic::initLevel( ) {
	destroyLevel( );

	// Init syncronization
	player_sync = init_player_sync;

	// Init
	serotoninaRemaining    = init_serotoninaRemaining; // TODO ENTIENDO QUE AQUÍ EN VEZ DE 3 HABRÁ UN  init_serotoninaRemaining DE XML
	noradrenalinaRemaining = init_noradrenalinaRemaining; // TODO ENTIENDO QUE AQUÍ EN VEZ DE 3 HABRÁ UN  init_noradrenalinaRemaining DE XML

	current_syn_player_lose_per_second = syn_player_lose_per_second;

	state = 1;
	currentTipo = -1;

	D3DXVECTOR3 p;
	
	BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( transSerotonina, 0.0f,  1.4666f, -1.345f);
	BrainManager::get( )->getCurrentBrain( ).getPointOnPlanet(p, 1.4666f, -1.345f);
	Global::triggers->addLogicVolume( "zona_plantar_serotonina", p, radius_zona_plantar );
	PSManager::get( )->addParticleGroup( "blue_bomb_place", TTracker( transSerotonina ) );

	BrainManager::get( )->getCurrentBrain( ).getPointOnPlanet(p, 4.91f, -1.26f);
	BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( transNoradrenalina, 0.0f,  4.91f, -1.26f);
	Global::triggers->addLogicVolume( "zona_plantar_noradrenalina", p, radius_zona_plantar);
	PSManager::get( )->addParticleGroup( "red_bomb_place", TTracker( transNoradrenalina ) );

	createBomba();

	Global::triggers->addUserEvent( "no_player_control_for_a_while" );
	if( show_dh_first_time_show_planet_gameplay ) {
		show_dh_first_time_show_planet_gameplay = false;
		Global::triggers->addLogicTimer( "show_dinamic_help( \"PLANET2_FIRST_MESSAGE\" )", time_to_show_planet_gameplay_msg, LogicTimer::COMMAND );
	}

	BrainManager::get()->getCurrentBrain().CHANGETEXTUREANDNORMAL("brain2_1.dds", "brain_normalmap.dds");
}


void SecondLevelLogic::createBomba()
{

	assert(bomba == NULL);

	if (serotoninaRemaining == 0)
	{
		currentTipo = NORADRENALINA;
	}
	else if (noradrenalinaRemaining == 0)
	{
		currentTipo = SEROTONINA;
	}
	else
	{
		int tipo = (int) obtainRandom(0.0f, 50.0f);
		currentTipo = tipo % 2;
	}

	std::string s;

	if (currentTipo == SEROTONINA)
	{
		s = "bomba_serotonina";
	}
	else
	{
		s = "bomba_noradrenalina";
	}


	const IntelligentObject &bomba_to_add = EnemyManager::get( )->getEnemy( s );
	bomba = new IntelligentObject(bomba_to_add);

	bomba->loc.setAlpha(alpha_nacimiento);
	bomba->loc.setDelta(delta_nacimiento);

	D3DXMATRIX transform;
	BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( transform, 0.0f, alpha_nacimiento, delta_nacimiento );
	bomba->loc.setTransform(transform);

	Global::spatial.addNonSpatialFixObject( bomba );
	

	ParticleGroup *pgroup = PSManager::get( )->addParticleGroup( "bomb_fuse", TTracker( &bomba->loc ) );
	bomba->setParticleGroup( pgroup );
	
}

void SecondLevelLogic::update( float deltaTime ) {

	//// El player va perdiendo vida a medida que pasa el tiempo
	playerSyncDown( deltaTime * current_syn_player_lose_per_second);

	processRayos(deltaTime);
	if (previousState != state)
	{
		changeTexture();
		previousState = state;

	}

	if (mustDeleteBomba && bomba)
	{
		Global::spatial.removeObject( bomba ); // Remove it to the spatial
		delete bomba, bomba = NULL;
		createBomba();
		mustDeleteBomba = false;
	}

	// PowerUps
	time_to_born_powerup += deltaTime;
	if( time_to_born_powerup >= max_time_to_born_powerup ) {
		time_to_born_powerup -= max_time_to_born_powerup;
		power_ups.add( 1 );
	}
}

void SecondLevelLogic::loadLogic( MKeyValue &atts ) {
	init_player_sync			      = atts.getFloat( "init_player_sync", 250.0f );
	syn_player_lose_per_second	= atts.getFloat( "syn_player_lose_per_second", 1.0f );
	init_noradrenalinaRemaining = atts.getInt("init_noradrenalinaRemaining", 3);
	init_serotoninaRemaining = atts.getInt("init_serotoninaRemaining", 3);
	radius_zona_plantar = atts.getFloat("radius_zona_plantar", 0.5f);
	alpha_nacimiento = atts.getFloat("alpha_nacimiento", 0.0f);
	delta_nacimiento = atts.getFloat("delta_nacimiento", 1.0f);
	tiempo_rayos = atts.getFloat("tiempo_rayos", 10.0f);
	time_to_show_planet_gameplay_msg = atts.getFloat("time_to_show_planet_gameplay_msg", 1.0f);
	minRandomLeft = atts.getFloat("minRandomLeft", -1.0f);
	maxRandomLeft = atts.getFloat("maxRandomLeft", 1.0f);
	minRandomFront = atts.getFloat("minRandomFront", -1.0f);
	maxRandomFront = atts.getFloat("maxRandomFront", 5.0f);

	// Load powerups
	max_time_to_born_powerup                  = atts.getFloat( "max_time_to_born_powerup", 1.0f );
	const std::string &powerups_positions_xml = atts[ "powerups_positions_xml" ];
	const std::string &pathToXML              = Global::data_locator.getPath( "xml" );
	power_ups.xmlParseFile( pathToXML + powerups_positions_xml );
}

void SecondLevelLogic::resetDinamicHelp( ) {
	must_show_welcome_image = true;

	show_dh_first_time_gets_bomb = true;
	show_dh_first_time_explodes_planted = true;
	show_dh_first_time_show_planet_gameplay = true;
}

void SecondLevelLogic::processRayos(float deltaTime)
{
	rayos.update(deltaTime);

	timer_rayos+=deltaTime;
	if (timer_rayos > new_tiempo_rayos)
	{
		rayos.add(minRandomLeft, maxRandomLeft, minRandomFront, maxRandomFront);
		timer_rayos = 0.0f;

		new_tiempo_rayos = obtainRandom(tiempo_rayos, tiempo_rayos + 1.0f);
	}	
}

void SecondLevelLogic::dentro_zona_plantar_serotonina()
{
	if (bomba == NULL)
		return;

	Follower *f = (Follower *) bomba->getAIController();

	if (f->GetStatus() == Follower::FOLLOWING && currentTipo == SEROTONINA)
	{
		f->SetStatus(Follower::PLANTED);
		bomba->loc.setTransform(transSerotonina);
	}
}

void SecondLevelLogic::dentro_zona_plantar_noradrenalina()
{
	if (bomba == NULL)
		return;

	Follower *f = (Follower *) bomba->getAIController();

	if (f->GetStatus() == Follower::FOLLOWING && currentTipo == NORADRENALINA)
	{
		Follower *f = (Follower *) bomba->getAIController();
		f->SetStatus(Follower::PLANTED);
		bomba->loc.setTransform(transNoradrenalina);
	}
}

void SecondLevelLogic::showGetBombDinamicHelp()
{
	if( idSonido != 0 )
		SoundPlayer::get( )->stopSample(idSonido); // Esto no deberia pasar... pero por seguridad...
	idSonido = SoundPlayer::get( )->playSample( "bomb_fuse", true );

		if( show_dh_first_time_gets_bomb ) {
			show_dh_first_time_gets_bomb = false;
			IGameState::showDinamicHelp( DinamicHelp::PLANET2_PLAYER_GETS_BOMB );
		}
}

void SecondLevelLogic::exploding(IntelligentObject &object, int status, int points_score_up)
{
	SoundPlayer::get( )->stopSample(idSonido);
	idSonido = 0;

	if( currentTipo == SEROTONINA )
		PSManager::get( )->addParticleGroup( "blue_bomb_explosion", TTracker( bomba->loc.getTransform() ) );
	else
		PSManager::get( )->addParticleGroup( "red_bomb_explosion", TTracker( bomba->loc.getTransform() ) );

	SoundPlayer::get( )->playSample( "bomb_explosion" );

	SSPlayerCameraController *cc = (SSPlayerCameraController *)getCamera3D().getCameraController();

	if (cc)
		cc->vibrateDuring( 0.5f, SSPlayerCameraController::LOW );

	if (status == Follower::PLANTED)
	{
		incrementarState();

		if (currentTipo == SEROTONINA)
		{
			serotoninaRemaining--;
		}
		else
		{
			noradrenalinaRemaining--;
		}

		Logic::scoreUp( points_score_up );

		SSPlayerCameraController *cc = (SSPlayerCameraController *)getCamera3D().getCameraController();

		if( show_dh_first_time_explodes_planted ) {
			show_dh_first_time_explodes_planted = false;
			IGameState::showDinamicHelp( DinamicHelp::PLANET2_PLAYER_LEAVES_BOMB );
		}

		if (cc)
			cc->vibrateDuring( 0.5f, SSPlayerCameraController::MEDIUM );
	}
	else
	{
		const D3DXVECTOR3 &player_pos = Global::player->loc.getPosition( );
		const D3DXVECTOR3 &obj_pos = object.loc.getPosition( );
		D3DXVECTOR3 hit_dir;

		if( player_pos == obj_pos )
			hit_dir = -object.loc.getLocalFront( );
		else {
			hit_dir = player_pos - obj_pos;
			normalizeVector3( hit_dir, hit_dir );
		}
		Global::player->processSignal( SIGNAL_HIT_PLAYER, &hit_dir, sizeof( hit_dir ) );

		playerSyncDown(25.0f);

		if (cc)
			cc->vibrateDuring( 0.5f, SSPlayerCameraController::LOW );
	}

	mustDeleteBomba = true;
}

void SecondLevelLogic::changeTexture()
{
	std::string s = "";

	switch (state)
	{
	case 1:
		s = "brain2_1.dds";
		break;
	case 2:
		s = "brain2_2.dds";
		break;
	case 3:
		s = "brain2_3.dds";
		break;
	case 4:
		s = "brain2_4.dds";
		break;
	case 5:
		s = "brain2_5.dds";
		break;
	case 6:
		s = "brain2_6.dds";
		break;
	case 7:
		s = "brain2_6.dds";
		break;
	default:
		assert( !"This should not happend!" );
		return;
	}

	BrainManager::get()->getCurrentBrain().CHANGETEXTUREANDNORMAL(s, "brain_normalmap.dds");

}


float SecondLevelLogic::GetTimerBomba()
{
	if (bomba == NULL)
		return -1.0f;

	Follower *f = (Follower *) bomba->getAIController();
	
	if (f->GetStatus() != Follower::FOLLOWING)
		return -1.0f;

	return f->getTimeRemaining();
}

void SecondLevelLogic::playerInsideResidual(float deltaTime)
{
	playerSyncDown( deltaTime * current_syn_player_lose_per_second * 2.0f);
}

float SecondLevelLogic::getRotationToFollowZona() {
	const D3DXVECTOR3 &bomba_pos = bomba->loc.getPosition( );

	D3DXVECTOR3 zona_pos;

	if (currentTipo == NORADRENALINA)
	{
		zona_pos.x = transNoradrenalina(3,0);
		zona_pos.y = transNoradrenalina(3,1);
		zona_pos.z = transNoradrenalina(3,2);
	}
	else
	{
		zona_pos.x = transSerotonina(3,0);
		zona_pos.y = transSerotonina(3,1);
		zona_pos.z = transSerotonina(3,2);
	}

	if (distance( zona_pos, bomba->loc.getPosition()) < 0.01f)
		return 0.0f;

	D3DXVECTOR3 dir_to_zona = zona_pos - bomba_pos;
	normalizeVector3( dir_to_zona, dir_to_zona );

	// Get angle
	float angle = D3DXVec3Dot( &(bomba->loc.getLocalFront( )), &dir_to_zona );
	angle = acos( angle );
	if( angle == 0.0f )
		return 0.0f;

	// Get sign of angle with a plane test
	// Ax + By + Cz + D = 0
	// (A,B,C) -> getLocalLeft
	// (x,y,z) -> dir_to_player
	const D3DXVECTOR3 &obj_left = bomba->loc.getLocalLeft( );
	float sign = obj_left.x * dir_to_zona.x + obj_left.y * dir_to_zona.y + obj_left.z * dir_to_zona.z;

	return ( sign < 0.0f ) ? -angle : angle;
}