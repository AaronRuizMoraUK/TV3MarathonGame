#include "LogosState.h"
#include "Global/Rendering.h"
#include "IO/IOStatus.h"
#include "Game/Game.h"

LogosState::LogosState()
{
}

LogosState::~LogosState() {
	printDebugString("Destroying LogosState...\n");
}

void LogosState::update( float deltaTime ) {

	bool finished = texture_sequencer.update( deltaTime );
	if( finished ) {
		getState( "menu_state" )->getFocus( );
		return;
	}

	if( ioStatus.start.becomesPressed() || ioStatus.selectOption.becomesPressed() || ioStatus.exitOptions.becomesPressed( ) )
		texture_sequencer.nextScreen( );
}

void LogosState::preRender( ) {
}

void LogosState::renderWorld3D( ) {
}

void LogosState::renderWorld2D( ) {
}

void LogosState::renderPostProcessed2D( ) {
}

void LogosState::renderGameUI( ) {
	renderStateEnableAlphaBlending();

	texture_sequencer.render( );

	renderStateDisableAlphaBlending();
}

void LogosState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Logos Status");
#endif
}

void LogosState::onLoad( ) {
	texture_sequencer.initiate( );
}

void LogosState::onUnload( ) {
}

void LogosState::loadGameState( ) {
	texture_sequencer.load( "logos.xml" );
}
