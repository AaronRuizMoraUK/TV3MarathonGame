#include "FirstLevelState.h"
#include "EndFirstLevelState.h"
#include "GameState/Game/ScoreRecountState.h"
#include "IO/IOStatus.h"
#include "Camera/Camera3D.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"
#include "Source/Game.h"
#include "Logic/FirstLevelLogic.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Factories/QuadFactory.h"

const float FirstLevelState::max_hurt_time = 1.0f;

FirstLevelState::FirstLevelState()
: hurt_time( 0.0f )
{
}

FirstLevelState::~FirstLevelState() {
	printDebugString("Destroying FirstLevelState...\n");
}

void FirstLevelState::update( float deltaTime ) {
	// GANAR
	if( getFirstLevelLogic( ).isLevelWon( )
#ifndef NDEBUG
		|| isPushed( 'G' )
#endif
		) {
		// Next go to Score Recount
		EndFirstLevelState *end_first_level_state = (EndFirstLevelState *)getState( "end_first_level_state" );
		end_first_level_state->next_instruction = InstructionsState::SCORE_RECOUNT;

		// Prepare that after Score Recount, we go to Init Second Level
		ScoreRecountState *score_recount_state = (ScoreRecountState *)getState( "score_recount_state" );
		score_recount_state->next_instruction = InstructionsState::INIT_SECOND_LEVEL;

		// Go to End First Level State
		end_first_level_state->getFocus( );
		return;
	}
	// PERDER
	else if( getFirstLevelLogic( ).isLevelLost( ) 
#ifndef NDEBUG
		|| isPushed( 'P' ) 
#endif
		) {
		// Bajar una vida
		Logic::lifeDown( );

		// Hemos acabado el juego
		EndFirstLevelState *end_first_level_state = (EndFirstLevelState *)getState( "end_first_level_state" );
		if( Logic::isGameOver( ) )
			end_first_level_state->next_instruction = InstructionsState::GAME_OVER;
		else
			end_first_level_state->next_instruction = InstructionsState::LOSE_FIRST_LEVEL;
		end_first_level_state->getFocus( );
		return;
	}
	// PAUSE
	else if( ioStatus.start.becomesPressed() ) {
		showPause( "end_first_level_state" );
		return;
	}

	// -------------------------------------------

	// Update camera
	getCamera3D( ).update(deltaTime);

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	// Update Controllers
	Global::player->updateController(deltaTime);

	// Update AIs
	getFirstLevelLogic( ).dopaminas.updateAI( deltaTime );
	getFirstLevelLogic( ).alucinaciones.updateAI( deltaTime );
	getFirstLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getFirstLevelLogic( ).alucinaciones.updateAnimation( deltaTime );
	getFirstLevelLogic( ).power_ups.updateAnimation( deltaTime );

	// Spatial and Collisions
	Global::spatial.update( deltaTime );
	Global::collisions.update( deltaTime );

	// Update Logic and triggers
	getFirstLevelLogic( ).update( deltaTime );
	Global::triggers->update( deltaTime );

	// Update Particles
	PSManager::get( )->update( deltaTime );

	// Update UI
	first_level_ui.update( deltaTime );

	// Update Hurt
	if( hurt_time > 0.0f )
		hurt_time -= deltaTime;
}

void FirstLevelState::preRender( ) {
}

void FirstLevelState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	Global::player->render();
	BrainManager::get( )->getCurrentBrain( ).render();
	Global::sky.render();
	getFirstLevelLogic( ).alucinaciones.render( );

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void FirstLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	getFirstLevelLogic( ).dopaminas.render( );
	getFirstLevelLogic( ).power_ups.render( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void FirstLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	if( hurt_time > 0.0f )
		hurt.render( hurt_time/max_hurt_time );

	renderStateDisableAlphaBlending( );
}

void FirstLevelState::renderGameUI( ) {
	renderStateEnableAlphaBlending( );

	first_level_ui.render( );

	renderStateDisableAlphaBlending( );
}

void FirstLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 170;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0,  py, 0xffff0000, "First Level Status");
	py += 10;

	py += printf(0, py, 0xffff0000, "Num Dopaminas %d", getFirstLevelLogic( ).dopaminas.size( ) );
	py += printf(0, py, 0xffff0000, "Num Alucinaciones %d", getFirstLevelLogic( ).alucinaciones.size( ) );
	py += printf(0, py, 0xffff0000, "Right Dopamine %s", getFirstLevelLogic( ).isDopaminaLevelRight( ) ? "TRUE" : "FALSE" );
	py += 10;

	py += printf(0, py, 0xffff0000, "Num PowerUps %d", getFirstLevelLogic( ).power_ups.size( ) );
	py += printf(0, py, 0xffff0000, "Num PowerUps can be added %d", getFirstLevelLogic( ).power_ups.getMaxPowerUpsSameTime( ) - getFirstLevelLogic( ).power_ups.size( ) );
	py += printf(0, py, 0xffff0000, "Num PowerUps Born Points %d", getFirstLevelLogic( ).power_ups.getNumPowerUpsBornPoints( ) );
	py += printf(0, py, 0xffff0000, "Num Non Used PowerUps Born Points %d", getFirstLevelLogic( ).power_ups.getNumNonUsedPowerUpsBornPoints( ) );
	py += printf(0, py, 0xffff0000, "Num Used PowerUps Born Points %d", getFirstLevelLogic( ).power_ups.getNumUsedPowerUpsBornPoints( ) );

	// Spatial Information
	//size_t grid = Global::spatial.getGridOfObject( Global::player );
	//py += printf(0, py, 0xffff0000, "Player Grid %d", grid );
	//py += printf(0, py, 0xffff0000, "Num Grids %d", SpatialManager::getNumGrids( ) );

	//size_t total = 0;
	//py += printf(0, py, 0xffff0000, "Movable Object Grids" );
	//for( size_t i=0; i<SpatialManager::getNumGrids( ); ++i ) {
	//	py += printf(0, py, 0xffff0000, "\tGrid %d: %d", i, Global::spatial.sizeMovableObjects( i ) );
	//	total += Global::spatial.sizeMovableObjects( i );
	//}
	//py += printf(0, py, 0xffff0000, "Spatial Fix Object Grids" );
	//for( size_t i=0; i<SpatialManager::getNumGrids( ); ++i ) {
	//	py += printf(0, py, 0xffff0000, "\tGrid %d: %d", i, Global::spatial.sizeSpatialFixObjects( i ) );
	//	total += Global::spatial.sizeSpatialFixObjects( i );
	//}
	//py += printf(0, py, 0xffff0000, "Non Spatial Fix Objects: %d", Global::spatial.sizeNonSpatialFixObjects( ) );
	//	total += Global::spatial.sizeNonSpatialFixObjects( );
	//py += printf(0, py, 0xffff0000, "All Objects: %d", Global::spatial.sizeAllObjects( ) );
	//py += printf(0, py, 0xffff0000, "My Total: %d", total );
	//assert( total == Global::spatial.sizeAllObjects( ) );
#endif
}

void FirstLevelState::onLoad( ) {
	first_level_ui.reset( );
	if( getFirstLevelLogic( ).must_show_welcome_image ) {
		getFirstLevelLogic( ).must_show_welcome_image = false;
		first_level_ui.showWelcomeImage( );
	}

	hurt_time = 0.0f;
}

void FirstLevelState::onUnload( ) {
}

void FirstLevelState::loadGameState( ) {
	// Vignetting
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		vignetting.SETMATERIAL("vignetting_pan.dds", "shader.fx", "dummy_tech");
	else
		vignetting.SETMATERIAL("vignetting.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	vignetting.setMesh(mesh);

	// Hurt
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		hurt.load("hurt_pan.dds");
	else
		hurt.load("hurt.dds");
	D3DXMATRIX hurt_transform;
	D3DXMatrixScaling( &hurt_transform, aspect_ratio, 1.0f, 1.0f );
	hurt.setTransform( hurt_transform );

	first_level_ui.load( );
}
