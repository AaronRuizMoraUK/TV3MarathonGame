#include <cassert>
#include "TestState.h"
#include "IO/IOStatus.h"
#include "Game/Game.h"
#include "Camera/ICamera.h"
#include "Camera/Camera3D.h"
#include "Factories/QuadFactory.h"
#include "Factories/MeshFactory.h"
#include "Global/Rendering.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Managers/PlanetMapManager.h"
#include "AnimationControllers/ROA/ROAObject.h"
#include "Managers/ROAResourceManager.h"
#include "CameraControllers/ROA/ROACamera.h"
#include "Object/AnimatedObject/CalAnimModel.h"
#include "Managers/CalAnimCoreModelManager.h"
#include "Global/Geometry.h"
#include "Global/Variables.h"
#include "AnimationControllers/YAxisRotation/YAxisRotation.h"

void testSoundPlayer( ) {
	SoundPlayer *sound_player = SoundPlayer::get( );
	if( ioStatus.kick.becomesPressed( ) )
		sound_player->playSample( "normal" );
	if( ioStatus.enterOptions.becomesPressed( ) )
		sound_player->playSample( "player_expeled_by_ass_1" );
	if( ioStatus.walk.becomesPressedUp( ) )
		sound_player->playSample( "player_being_swallowed" );
	if( ioStatus.walkSlow.becomesPressed( ) )
		sound_player->stopAll( true );

	if( isPushed( 'P' ) )
		sound_player->stopAll( );
}

// To test planet's functions
D3DXVECTOR3 test_point;
float alpha = 0;
float delta = 0;
float polar_alpha, polar_delta;
D3DXVECTOR3 point_on_planet;
D3DXVECTOR3 point_on_planet_own;
float height;
D3DXVECTOR3 normal_on_planet;

void testPlanet( Planet &planet, float deltaTime ) {
	static const float speed = 1.0f;
	if( ioStatus.walk.isPressedUp( ) )
		delta += deltaTime * speed;
	if( ioStatus.walk.isPressedDown( ) )
		delta -= deltaTime * speed;
	if( ioStatus.walk.isPressedRight( ) )
		alpha += deltaTime * speed;
	if( ioStatus.walk.isPressedLeft( ) )
		alpha -= deltaTime * speed;

	anglesToVector( alpha, delta, test_point );
	planet.getPolarAngles( test_point, polar_alpha, polar_delta );
	planet.getPointOnPlanet( point_on_planet, polar_alpha, polar_delta );
	height = planet.getHeight( polar_alpha, polar_delta );
	planet.getNormal( normal_on_planet, polar_alpha, polar_delta );

	D3DXVECTOR3 direction = test_point - planet.getCenter( );
	normalizeVector3( direction, direction );
	point_on_planet_own = planet.getCenter( ) + height * direction;
}

ROAObject *createROAObject( const char *anim, TLocation &loc ) {
	const ROAResource *resource = ROAResourceManager::get( )->getResource( anim );
	return new ROAObject( &resource->rigidAnimationResource
		, loc.getPosition( ), loc.getRotation( ), true );
}

ROACamera *createROACamera( const char *anim, TLocation &loc ) {
	const ROAResource *resource = ROAResourceManager::get( )->getResource( anim );
	return new ROACamera( &resource->rigidAnimationResource
		, loc.getPosition( ), loc.getRotation( ), false );
}

void testROAObject( AnimatedObject &player, float deltaTime ) {
	if( ioStatus.walk.becomesPressedUp( ) )
		player.setAnimationController( createROAObject( "menu_next.xml", player.loc ) );
	else if( ioStatus.walk.becomesPressedDown( ) )
		player.setAnimationController( createROAObject( "menu_previous.xml", player.loc ) );
	player.updateAnimation( deltaTime );
}

void testROACamera( float deltaTime ) {
	if( ioStatus.walk.becomesPressedRight( ) )
		getCamera3D( ).setCameraController( createROACamera( "menu_next.xml", getCamera3D( ).getLocation( ) ) );
	else if( ioStatus.walk.becomesPressedLeft( ) )
		getCamera3D( ).setCameraController( createROACamera( "menu_previous.xml", getCamera3D( ).getLocation( ) ) );
	getCamera3D( ).update( deltaTime );
}

ROACamera *createROAPoint( TLocation &loc, const D3DXVECTOR3 &end_point ) {
	RigidAnimationResource *anim = new RigidAnimationResource( );
	D3DXQUATERNION rot;
	D3DXQuaternionRotationYawPitchRoll( &rot, -D3DX_PI/4.0f, D3DX_PI/4.0f, 0.0f );
	anim->initLinear( loc.getPosition( ), loc.getRotation( ), end_point, rot, 3.0f );
	return new ROACamera( anim, loc.getPosition( ), loc.getRotation( ), false );
}

void testROAPoint( float deltaTime ) {
	if( ioStatus.enterOptions.becomesPressed( ) )
		getCamera3D( ).setCameraController( createROAPoint( getCamera3D( ).getLocation( ), D3DXVECTOR3(0,3,-1) ) );
	getCamera3D( ).update( deltaTime );
}

void testCal( CalAnimatedObject &player, float deltaTime ) {
	if( ioStatus.walkSlow.becomesPressed( ) )
		player.getCalAnimModel( )->playCycle( "idle" );
	if( ioStatus.walk.becomesPressedUp( ) )
		player.getCalAnimModel( )->playCycle( "transition" );
	if( ioStatus.walk.becomesPressedRight( ) )
		player.getCalAnimModel( )->playAction( "jumping" );
	if( ioStatus.walk.becomesPressedLeft( ) )
		player.getCalAnimModel( )->removeAction( "jumping" );
	player.updateAnimation( deltaTime );
}

void testLua( ) {
	if( ioStatus.kick.becomesPressed( ) ) {
		Global::lua_vm.executeCmd( "print( \"Hola LUA\" )" );
	}
}

size_t curr_score_to_add = 0;
void testScoreRecorder( ) {
	if( ioStatus.walk.isPressedUp( ) )
		++curr_score_to_add;
	else if( ioStatus.walk.isPressedDown( ) && curr_score_to_add>0 )
		--curr_score_to_add;

	if( ioStatus.walkSlow.becomesPressed( ) ) {
		//static char initials[4];
		//static int num = 0;
		//++num;
		//sprintf_s( initials, sizeof( initials ), "A%d", num );
		Global::score_recorder.addScore( "ARM", curr_score_to_add );
	}
	else if( ioStatus.walk.becomesPressedRight( ) )
		Global::score_recorder.saveScoreInfos( );
	else if( ioStatus.walk.becomesPressedLeft( ) )
		Global::score_recorder.loadScoreInfos( );
}

// -----------------------------------------------------------------------------


TestState::~TestState() {
	printDebugString("Destroying TestState...\n");
}

void TestState::update( float deltaTime ) {
	if( isPushed( 'Q' ) ) {
		Game::exitGame( );
		return;
	}

	//// -------------------------------------
	//D3DXVECTOR3 save_pos = player2.loc.getPosition( );
	//D3DXMATRIX transform = player2.loc.getTransform( );
	//transform(3,0) = transform(3,1) = transform(3,2) = 0.0f;

	//D3DXMATRIX rot;
	//D3DXMatrixRotationAxis( &rot, &D3DXVECTOR3(0,0,1), deltaTime*0.5f );
	//D3DXMatrixMultiply( &transform, &transform, &rot );

	//transform(3,0) = save_pos.x;
	//transform(3,1) = save_pos.y;
	//transform(3,2) = save_pos.z;

	//player2.loc.setTransform( transform );
	// -------------------------------------
	//D3DXMATRIX transform;
	//rotateOnAxis( transform, player2.loc.getTransform( ), D3DXVECTOR3(0,0,1), deltaTime );
	//player2.loc.setTransform( transform );

	//testSoundPlayer( );
	//testPlanet( planet, deltaTime );
	//testROAObject( player, deltaTime );
	//testROACamera( deltaTime );
	//testROAPoint( deltaTime );
	//testCal( player2, deltaTime );
	//testLua( );
	testScoreRecorder( );
}

void TestState::preRender( ) {
	// --- Debug draws --- //
	activeMatsOnDeviceForDrawsUP( );
	drawGridXZ( );
	drawAxis( );

	D3DXMATRIX matrix;
	D3DXMatrixIdentity( &matrix );
	matrix(3,0) = test_point.x;
	matrix(3,1) = test_point.y;
	matrix(3,2) = test_point.z;
	Global::device->SetTransform( D3DTS_WORLD, &matrix );
	drawAxis( 0.2f );
}

void TestState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	player2.render( );
	//planet.render( );

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void TestState::renderWorld2D( ) {
	//renderStateEnableAlphaBlending( );
	//renderStateEnableZTest( );

	//renderStateDisableZTest( );
	//renderStateDisableAlphaBlending( );
}

void TestState::renderPostProcessed2D( ) {
	//quad.render( );
	//quad2.render( );
}

void TestState::renderGameUI( ) {
}

void TestState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0,  0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Test Status");
	py += 10;

	//py += printf(0, py, 0xffff0000, "Stream Volume: %d", SoundPlayer::get( )->getStreamsVolume( ) );
	//py += printf(0, py, 0xffff0000, "Sample Volume: %d", SoundPlayer::get( )->getSamplesVolume( ) );
	//py += 10;

	//py += printf(0, py, 0xffff0000, "Alpha: %f Delta: %f", alpha, delta );
	//py += printf(0, py, 0xffff0000, "Polar Alpha: %f Delta: %f", polar_alpha, polar_delta );
	//py += printf(0, py, 0xffff0000, "Point on planet: (%f, %f, %f)", point_on_planet.x, point_on_planet.y, point_on_planet.z );
	//py += printf(0, py, 0xffff0000, "Point on planet own: (%f, %f, %f)", point_on_planet_own.x, point_on_planet_own.y, point_on_planet_own.z );
	//py += printf(0, py, 0xffff0000, "Height: %f", height );
	//py += printf(0, py, 0xffff0000, "Normal on planet: (%f, %f, %f)", normal_on_planet.x, normal_on_planet.y, normal_on_planet.z );

	py += printf( 0, py, 0xffff0000, "Score to Add: %d (%s)", curr_score_to_add, Global::score_recorder.isInsideTopScores( curr_score_to_add ) ? "TRUE" : "FALSE" );
	py += printf( 0, py, 0xffff0000, "Num score Infos: %d", Global::score_recorder.getNumScoreInfos( ) );
	for( size_t i=0; i<Global::score_recorder.getNumScoreInfos( ); ++i ) {
		const ScoreRecorder::ScoreInfo &score_info = Global::score_recorder.getScoreInfo( i );
		int day, month, year;
		score_info.getDate( day, month, year );
		py += printf( 0, py, 0xffff0000, "%d) \"%s\" %d - %02d/%02d/%04d", i, score_info.initials, score_info.score, day, month, year );
	}

	//print( player2.getCalAnimModel( ) );
#endif
}

void TestState::onLoad( ) {
	getCamera3D( ).lookAt( D3DXVECTOR3(0,2,5), D3DXVECTOR3(0,0,0) );
}

void TestState::onUnload( ) {
}

void TestState::loadGameState( ) {
	const IMesh * quad_mesh = QuadFactory::get()->createQuad(0.5, 0.5f);
	assert( quad_mesh );
	quad.setMesh( quad_mesh );
	quad.SETMATERIAL("xbox_buttons.dds", "shader.fx", "dummy_tech");
	quad.loc.setPosition( D3DXVECTOR3( 0.4f, 0.0f, 0.0f ) );

	// -----------------------------------
	const IMesh * quad2_mesh = QuadFactory::get()->createQuad(0.3f, 0.3f);
	assert( quad2_mesh );
	quad2.setMesh( quad2_mesh );
	quad2.SETMATERIAL("loading_splash.dds", "shader.fx", "dummy_tech");
	quad2.loc.setPosition( D3DXVECTOR3( 0.4f, 0.0f, 0.0f ) );

	// -----------------------------------
	const IMesh * player_mesh = MeshFactory::get()->createMesh( "player.mesh" );
	assert( player_mesh );
	player.setMesh( player_mesh );
	player.SETMATERIAL("player_skin.dds", "shader.fx", "basic_tech");
	player.loc.setPosition( D3DXVECTOR3( -1.0f, 0.0f, 0.0f ) );

	// -----------------------------------
	player2.setCalAnimModel( "player" );
	player2.SETMATERIAL("player_skin.dds", "shader.fx", "skinning_tech");
	player2.getCalAnimModel( )->playCycle( "idle" );

	//D3DXMATRIX rot;
	//D3DXMatrixRotationYawPitchRoll( &rot, 0, D3DX_PI, 0 );
	//player2.loc.setTransform( rot );
	player2.loc.setPosition( D3DXVECTOR3( -1.0f, 0.0f, 0.0f ) );
	player2.setAnimationController( new YAxisRotation( 3.0f ) );

	// -----------------------------------
	const IMesh * planet_mesh = MeshFactory::get()->createMesh( "brain.mesh" );
	assert( planet_mesh );
	planet.setMesh( planet_mesh );
	planet.SETMATERIAL("brain.dds", "shader.fx", "normal_tech");
	planet.setName( "brain" );
	planet.setHeightMap( PlanetMapManager::get( )->getHeightMap( "brain_heightmap.bin" ) );
	planet.setNormalMap( PlanetMapManager::get( )->getNormalMap( "brain_normalmap.bin" ) );
}
