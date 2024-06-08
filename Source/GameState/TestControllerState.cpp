#include "TestControllerState.h"
#include "Source/Game.h"
#include "Bass/SoundPlayer.h"
#include "Camera/Camera3D.h"
#include "PlayerControllers/PlaneController/PlaneController.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Managers/CalAnimCoreModelManager.h"
#include "Factories/MeshFactory.h"
#include "AIControllers/ChaseController/ChaseController.h"

TestControllerState::TestControllerState( ) {
}

TestControllerState::~TestControllerState() {
	printDebugString("Destroying TestControllerState...\n");
}

void TestControllerState::update( float deltaTime ) {

	// Update Player Controller
	Global::player->updateController( deltaTime );

	// Update AI
	chaser.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation( deltaTime );
	chaser.updateAnimation( deltaTime );
}

void TestControllerState::preRender( ) {
	// --- Debug draws --- //
	activeMatsOnDeviceForDrawsUP( );
	drawGridXZ( );
	drawAxis( );
}

void TestControllerState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Pintar Objectos 3D
	Global::player->render( );
	chaser.render( );

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void TestControllerState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	// Pintar Objectos con alphas

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void TestControllerState::renderPostProcessed2D( ) {
}

void TestControllerState::renderGameUI( ) {
}

void TestControllerState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0,  0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Test Controller Status");
#endif
}

void TestControllerState::onLoad( ) {
	// Esto siempre se ejecuta cuando recibe el focus

	// Posicionar camara sinf controlador
	getCamera3D( ).lookAt( D3DXVECTOR3(0,6,13), D3DXVECTOR3(0,0,0) );
	getCamera3D( ).setCameraController( NULL );

	// Cargar controlador al player y ponerlo en Idle
	Global::player->loc.setPosition( D3DXVECTOR3( 5.0f, 0.0f, 0.0f) );
	Global::player->getCalAnimModel( )->clearAllAnimations( );
	Global::player->getCalAnimModel( )->playCycle( "idle" );
	PlaneController *plane_controller = new PlaneController( );
	Global::player->setPlayerController( plane_controller );
	
	// Cargar controlador al chaser y ponerlo a Idle
	chaser.loc.setPosition( D3DXVECTOR3( -5.0f, 0.0f, 0.0f) );
	chaser.getCalAnimModel( )->clearAllAnimations( );
	chaser.getCalAnimModel( )->playCycle( "idle" );
	ChaseController *chase_controller = new ChaseController( );
	chaser.setAIController( chase_controller );
}

void TestControllerState::onUnload( ) {
	// Esto siempre se ejecuta cuando pierde el focus
}

void TestControllerState::loadGameState( ) {
	// Esto se ejecuta al arrancar el juego.
	// Es el momento de cargar los objectos de este game state

	// Cargar el chaser
	chaser.setCalAnimModel( "arachnid" );
	chaser.SETMATERIAL("arachnid_r1.dds", "shader.fx", "skinning_tech");
}
