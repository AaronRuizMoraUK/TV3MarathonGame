#include "EmptyState.h"
#include "Global/Rendering.h"
#include "Game/Game.h"

EmptyState::EmptyState( ) {
}

EmptyState::~EmptyState() {
	printDebugString("Destroying EmptyState...\n");
}

void EmptyState::update( float deltaTime ) {
}

void EmptyState::preRender( ) {
}

void EmptyState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Pintar Objectos 3D

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void EmptyState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	// Pintar Objectos con alphas

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void EmptyState::renderPostProcessed2D( ) {
}

void EmptyState::renderGameUI( ) {
}

void EmptyState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0,  0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Empty Status");
#endif
}

void EmptyState::onLoad( ) {
	// Esto siempre se ejecuta cuando recibe el focus
}

void EmptyState::onUnload( ) {
	// Esto siempre se ejecuta cuando pierde el focus
}

void EmptyState::loadGameState( ) {
	// Esto se ejecuta al arrancar el juego.
	// Es el momento de cargar los objectos de este game state
}
