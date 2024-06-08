#include "ThirdLevelState.h"
#include "EndThirdLevelState.h"
#include "GameState/Game/ScoreRecountState.h"
#include "Logic/ThirdLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Camera/Camera3D.h"
#include "Source/Game.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Factories/MeshFactory.h"
#include "Factories/QuadFactory.h"
#include "Bass/SoundPlayer.h"

const float ThirdLevelState::max_hurt_time = 0.5f;

ThirdLevelState::ThirdLevelState()
: hurt_time( 0.0f )
{
}

ThirdLevelState::~ThirdLevelState() {
	printDebugString("Destroying ThirdLevelState...\n");
}

void ThirdLevelState::update( float deltaTime ) {
	// GANAR
	if( getThirdLevelLogic( ).isLevelWon( )
#ifndef NDEBUG
		|| isPushed( 'G' )
#endif
		) {
			EndThirdLevelState *end_third_level_state = (EndThirdLevelState *)getState( "end_third_level_state" );
			end_third_level_state->next_instruction = InstructionsState::SCORE_RECOUNT;

			// Prepare that after Score Recount, we go to Init Fourth Level
			ScoreRecountState *score_recount_state = (ScoreRecountState *)getState( "score_recount_state" );
			score_recount_state->next_instruction = InstructionsState::INIT_FOURTH_LEVEL;

			// Go to End Third Level State
			end_third_level_state->getFocus( );
			return;
	}
	// PERDER
	else if( getThirdLevelLogic( ).isLevelLost( ) 
#ifndef NDEBUG
		|| isPushed( 'P' ) 
#endif
		) {
			// Bajar una vida
			Logic::lifeDown( );

			// Hemos acabado el juego
			EndThirdLevelState *end_third_level_state = (EndThirdLevelState *)getState( "end_third_level_state" );
			if( Logic::isGameOver( ) )
				end_third_level_state->next_instruction = InstructionsState::GAME_OVER;
			else
				end_third_level_state->next_instruction = InstructionsState::LOSE_THIRD_LEVEL;
			end_third_level_state->getFocus( );
			return;
	}
	// PAUSE
	else if( ioStatus.start.becomesPressed() ) {
		showPause( "end_third_level_state" );
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
	getThirdLevelLogic( ).valves.updateAI( deltaTime );
	getThirdLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getThirdLevelLogic( ).valves.updateAnimation( deltaTime );
	getThirdLevelLogic( ).river.updateAnimation( deltaTime );
	getThirdLevelLogic( ).power_ups.updateAnimation( deltaTime );

	// Spatial and Collisions
	Global::spatial.update( deltaTime );
	Global::collisions.update( deltaTime );

	// Update Logic and triggers
	getThirdLevelLogic( ).update( deltaTime );
	Global::triggers->update( deltaTime );

	// Update Particles
	PSManager::get( )->update( deltaTime );

	// Update UI
	third_level_ui.update( deltaTime );

	// Update Hurt
	if( hurt_time > 0.0f )
		hurt_time -= deltaTime;
}

void ThirdLevelState::preRender( ) {
}

void ThirdLevelState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	Global::player->render();
	safe_path.render( );
	BrainManager::get( )->getCurrentBrain( ).render();
	Global::sky.render();
	
	getThirdLevelLogic( ).valves.render( );

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void ThirdLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	// Render
	getThirdLevelLogic( ).river.render( );
	getThirdLevelLogic( ).power_ups.render( );

	renderStateEnableAdditiveAlphaBlending( );
	renderStateDisableCulling( );
	getThirdLevelLogic( ).valves.renderGodrays( );
	renderStateEnableCulling( );
	renderStateEnableAlphaBlending( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void ThirdLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	if( hurt_time > 0.0f )
		hurt.render( hurt_time/max_hurt_time );

	renderStateDisableAlphaBlending( );
}

void ThirdLevelState::renderGameUI( ) {
	renderStateEnableAlphaBlending( );

	third_level_ui.render( );

	renderStateDisableAlphaBlending( );
}

void ThirdLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 170;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0,  py, 0xffff0000, "Third Level Status");
	py += 10;

	// Player on surface type
	float player_alpha = Global::player->loc.getAlpha( );
	float player_delta = Global::player->loc.getDelta( );
	Planet::SurfaceType surface_type = BrainManager::get( )->getCurrentBrain( ).getSurfaceType( player_alpha, player_delta );
	const char *surface_type_char;
	if( surface_type == Planet::SAFE )
		surface_type_char = "SAFE";
	else if( surface_type == Planet::DAMAGE )
		surface_type_char = "DAMAGE";
	else
		surface_type_char = "UNDEF";
	py += printf(0,  py, 0xffff0000, "Player on surface type: %s", surface_type_char );
	py += 10;

	// Valves Status
	py += print(0,  py, 0xffff0000, "Valves");
	const Valves::ValvesStatus &valves_status = getThirdLevelLogic( ).getValvesStatus( );
	for( size_t i=0; i<valves_status.size( ); ++i ) {
		py += printf(0,  py, 0xffff0000, "   %d) %s - %s - %0.3f", i
			, ValveController::status_chars[ valves_status[i].status ]
			, valves_status[i].right_status ? "YES" : "NO"
		  , valves_status[i].time_left );
	}
	py += 10;

	// River speed
	py += printf(0,  py, 0xffff0000, "River Speed: %0.3f", getThirdLevelLogic( ).river.getCurrentSpeed( ) );
	py += 10;

	py += printf(0, py, 0xffff0000, "Num PowerUps %d", getThirdLevelLogic( ).power_ups.size( ) );
#endif
}

void ThirdLevelState::onLoad( ) {
	third_level_ui.reset( );
	if( getThirdLevelLogic( ).must_show_welcome_image ) {
		getThirdLevelLogic( ).must_show_welcome_image = false;
		third_level_ui.showWelcomeImage( );
	}

	hurt_time = 0.0f;
}

void ThirdLevelState::onUnload( ) {
}

void ThirdLevelState::loadGameState( ) {
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

	third_level_ui.load( );

	// Safe Path
	const IMesh *safe_path_mesh = MeshFactory::get( )->createMesh( "safe_path_level3.mesh" );
	assert( safe_path_mesh );
	safe_path.setMesh( safe_path_mesh );
	safe_path.SETMATERIAL( "safe_path_level3.dds", "shader.fx", "basic_tech" );
}

void ThirdLevelState::playerHurt( ) {
	if( hurt_time <= 0.0f ) {
		SoundPlayer::get( )->playSample( "player_hit" );
		hurt_time = max_hurt_time;
	}
}
