#include <cassert>
#include "ScoreRecountState.h"
#include "IllnessState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Camera/ICamera.h"
#include "Game/Game.h"
#include "Factories/QuadFactory.h"
#include "IO/IOStatus.h"
#include "Logic/Logic.h"
#include "SoundPlayer/SoundPlayer.h"

const float ScoreRecountState::print_enter_time              = 0.2f;
const float ScoreRecountState::time_one_point_score_up       = 0.01f;
const float ScoreRecountState::sound_time_one_point_score_up = 0.1f;

ScoreRecountState::ScoreRecountState()
: next_instruction( InstructionsState::NONE )
, fading_in( true )
, fading_out( false )
, counting( true )
, curr_print_enter_time( 0.0f )
, print_enter( true )
, logic_score( 0 )
, logic_player_sync( 0 )
, time_score_up( 0.0f )
, sound_time_score_up( 0.0f )
, sound( 0 )
{
}

ScoreRecountState::~ScoreRecountState() {
	printDebugString("Destroying ScoreRecountState...\n");
}

void ScoreRecountState::update( float deltaTime ) {
	if ( fading_in ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else {
			fading_in = false;
			// Ejecutar sonido
			sound = SoundPlayer::get( )->playStream( "score_count", false );
		}
	}
	else if( fading_out ) {
		if( !Global::fade.isFinished( ) || ioStatus.selectOption.becomesPressed() )
			Global::fade.update( deltaTime );
		else {
			fading_out = false;

			// Go to instruction state
			//InstructionsState *instructions_state = (InstructionsState *) getState("instructions_state");
			//instructions_state->instruction = next_instruction;
			//instructions_state->getFocus();

			// Ir a la explicacion tecnica de la enfermedad mental
			IllnessState *illness_state = (IllnessState *) getState("illness_state");
			illness_state->next_instruction = next_instruction;
			illness_state->getFocus();
		}
	}
	else {
		if( counting ) {
			// Pasar cada X tiempo puntos de sync a score
			time_score_up += deltaTime;
			if( time_score_up >= time_one_point_score_up ) {
				time_score_up -= time_one_point_score_up;

				// Sumar al score un point de syn y quitarselo al sync
				logic_score += score_up_per_sync;
				--logic_player_sync;
			}

			// Lanzar sonido cada X tiempo
			sound_time_score_up += deltaTime;
			if( sound_time_score_up >= sound_time_one_point_score_up ) {
				sound_time_score_up -= sound_time_one_point_score_up;
				SoundPlayer::get( )->playSample( "score_recount" );
			}

			if( logic_player_sync == 0 || ioStatus.start.becomesPressed( ) || ioStatus.selectOption.becomesPressed() ) {
				// Sumar al score los puntos de sync que falten y poner sync a 0
				logic_score += logic_player_sync * score_up_per_sync;
				logic_player_sync = 0;

				SoundPlayer::get( )->playSample( "option_selected" );
				counting = false;
			}
		}
		else {
			if( ioStatus.start.becomesPressed( ) || ioStatus.selectOption.becomesPressed() ) {
				SoundPlayer::get( )->playSample( "option_selected" );
				SoundPlayer::get( )->stopStream( true ); // Parar sonido
				fading_out = true;
			}
		}
	}

	// Actualiar el parpadeo de ENTER msg
	curr_print_enter_time += deltaTime;
	if( curr_print_enter_time >= print_enter_time ) {
		curr_print_enter_time = print_enter_time - curr_print_enter_time;
		print_enter = !print_enter;
	}
}

void ScoreRecountState::preRender( ) {
	activeCameraOrtho( );
	background.render( );
	activeCamera3D( );
}

void ScoreRecountState::renderWorld3D( ) {
	//renderStateEnableZTest( );
	//renderStateEnableZWriting( );

	//renderStateDisableZWriting( );
	//renderStateDisableZTest( );
}

void ScoreRecountState::renderWorld2D( ) {
	//renderStateEnableAlphaBlending( );
	//renderStateEnableZTest( );

	//renderStateDisableZTest( );
	//renderStateDisableAlphaBlending( );
}

void ScoreRecountState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	//alphabet.print(0.43f, 0.665f, 0.08f, 0.08f, "SCORE");
	if( print_enter )
		alphabet.print( 0.63f, 0.1f, 0.03f, 0.03f, "ENTRAR - CONTINUAR" );

	renderStateDisableAlphaBlending( );
}

void ScoreRecountState::renderGameUI( ) {
	renderStateEnableAlphaBlending( );

	// Sync level
	sync_level_picture.render( );
	numbers.printf( 0.54f, 0.5f, 0.065f, 0.065f, "%3d", logic_player_sync );

	// Score
	score_picture.render( );
	size_t logic_score_to_print = logic_score;
	if( logic_score_to_print > MAX_SCORE_VALUE_TO_RENDER ) // Aunque se grabe un numero más alto, no se muestra más de 999999
		logic_score_to_print = MAX_SCORE_VALUE_TO_RENDER;
	numbers.printf( 0.29f, 0.49f, 0.048f, 0.048f, PRINTF_SCORE_CHAR, logic_score_to_print );

	renderStateDisableAlphaBlending( );
}

void ScoreRecountState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Score Recount Status");
#endif

	// Render fade medium out
	if( fading_in ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeIn(0.5f);
		renderStateDisableAlphaBlending( );
	}
	// Render fade in
	else if( fading_out ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeOut(0.5f);
		renderStateDisableAlphaBlending( );
	}
}

void ScoreRecountState::onLoad( ) {
	assert( next_instruction != InstructionsState::NONE );
	fading_in = true;
	fading_out = false;
	counting = true;

	curr_print_enter_time = 0.0f;
	print_enter = true;
	time_score_up = 0.0f;
	sound_time_score_up = 0.0f;

	sound = 0;

	// Init render logic values
	logic_score = Logic::getScore( );
	logic_player_sync  = (int)Logic::getPlayerSync( );

	// Update the logic
	Logic::scoreUp( logic_player_sync * score_up_per_sync );
}

void ScoreRecountState::onUnload( ) {
	next_instruction = InstructionsState::NONE;
	sound = 0;
}

void ScoreRecountState::loadGameState() {
	// Background
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		background.SETMATERIAL("score_recount_pan.dds", "shader.fx", "dummy_tech");
	else
		background.SETMATERIAL("score_recount.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	background.setMesh(mesh);

	// Sync level picture object
	const IMesh * sync_level_picture_mesh = QuadFactory::get()->createQuad(0.08f, 0.08f * Game::configOptions.resolution.aspect_ratio);
	assert(sync_level_picture_mesh);
	sync_level_picture.setMesh(sync_level_picture_mesh);
	sync_level_picture.loc.setPosition( D3DXVECTOR3( 0.32f, 0.0f, 0.0f ) );
	sync_level_picture.SETMATERIAL("sync_level.dds", "shader.fx", "dummy_tech");

	// Score picture object
	const IMesh * score_picture_mesh = QuadFactory::get()->createQuad(0.2f, 0.2f * Game::configOptions.resolution.aspect_ratio);
	assert(score_picture_mesh);
	score_picture.setMesh(score_picture_mesh);
	score_picture.loc.setPosition( D3DXVECTOR3( -0.3f, 0.0f, 0.0f ) );
	score_picture.SETMATERIAL("score.dds", "shader.fx", "dummy_tech");

	// Numbers
	numbers.load( );

	// Alphabet
	alphabet.load( );
}
