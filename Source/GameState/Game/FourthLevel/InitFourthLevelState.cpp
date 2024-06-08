#include "InitFourthLevelState.h"
#include "Logic/FourthLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "Camera/Camera3D.h"
#include "Game/Game.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Managers/ControllerManager.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Factories/QuadFactory.h"

InitFourthLevelState::InitFourthLevelState()
{
}

InitFourthLevelState::~InitFourthLevelState() {
	printDebugString("Destroying InitFourthLevelState...\n");
}

void InitFourthLevelState::update( float deltaTime ) {
	// Update Camera
	getCamera3D( ).update( deltaTime );

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	// Update AIs
	getFourthLevelLogic( ).dopaminas.updateAI( deltaTime );
	getFourthLevelLogic( ).noradrenalinas.updateAI( deltaTime );
	getFourthLevelLogic( ).twisters.updateAI( deltaTime );
	getFourthLevelLogic( ).expenders.updateAI( deltaTime );
	getFourthLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getFourthLevelLogic( ).expenders.updateAnimation( deltaTime );
	getFourthLevelLogic( ).power_ups.updateAnimation( deltaTime );

	// Spatial and Collisions
	Global::spatial.update( deltaTime );
	Global::collisions.update( deltaTime );

	// Update Particles
	PSManager::get( )->update( deltaTime );

	if( !Global::fade.isFinished( ) )
		Global::fade.update( deltaTime );
	else {
		getState("fourth_level_state")->getFocus();
		return;
	}
}

void InitFourthLevelState::preRender( ) {
}

void InitFourthLevelState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	Global::player->render();
	BrainManager::get( )->getCurrentBrain( ).render();
	Global::sky.render();
	
	getFourthLevelLogic( ).expenders.render( );

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void InitFourthLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	// Render
	getFourthLevelLogic( ).dopaminas.render( );
	getFourthLevelLogic( ).noradrenalinas.render( );
	getFourthLevelLogic( ).power_ups.render( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void InitFourthLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	renderStateDisableAlphaBlending( );
}

void InitFourthLevelState::renderGameUI( ) {
}

void InitFourthLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Init Fourth Level Status");
#endif

	renderStateEnableAlphaBlending( );
	Global::fade.renderFadeIn( 0.5f );
	renderStateDisableAlphaBlending( );
}

void InitFourthLevelState::onLoad( ) {
	// Sound
	SoundPlayer::get( )->playStream( "fourth_level" );

	// Brain
	BrainManager::get( )->setCurrentBrain( 4 );

	// Initial polar angles
	float alpha, delta;
	BrainManager::get( )->getCurrentBrain( ).getPolarAngles( D3DXVECTOR3(0,0,1), alpha, delta );
	//alpha += deg2rad( 5.0f ); // Nacer un poco a la derecha para tapar el fallo de los surcos al ir para arriba

	// Init Player Transform and Animation and Controller
	Global::player->loc.setAlpha( alpha );
	Global::player->loc.setDelta( delta );
	D3DXMATRIX transform;
	BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanetUsingCenter( transform, 0.0f, alpha, delta );
	Global::player->loc.setTransform( transform );
	Global::player->loc.setTransform( transform ); // Hacerlo 2 veces para que se actualice tb la prev_transform!!
	Global::player->setPlayerController( NULL );
	Global::player->getCalAnimModel( )->clearAllActions( );
	Global::player->getCalAnimModel( )->playCycle( "idle", 0.0f );
	IPlayerController *player_controller = ControllerManager::get( )->getPlayerController( "player_sphere" );
	assert( player_controller );
	Global::player->setPlayerController( player_controller );

	// Init camera position and Controller
	static const float init_camera_distance = 30.0f;
	const D3DXVECTOR3 &cam_pos = Global::player->loc.getPosition( );
	getCamera3D( ).lookAt( cam_pos + D3DXVECTOR3(0,0,1)*init_camera_distance, cam_pos );
	ICameraController *camera_controller = ControllerManager::get( )->getCameraController( "camera_sphere" );
	getCamera3D( ).setCameraController( camera_controller );

	// Inicializar logica
	getFourthLevelLogic( ).initLevel( );
}

void InitFourthLevelState::onUnload( ) {
}

void InitFourthLevelState::loadGameState( ) {
	// Vignetting
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		vignetting.SETMATERIAL("vignetting_pan.dds", "shader.fx", "dummy_tech");
	else
		vignetting.SETMATERIAL("vignetting.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	vignetting.setMesh(mesh);
}