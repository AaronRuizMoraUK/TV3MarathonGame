#include "SaveScoreState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Camera/ICamera.h"
#include "Game/Game.h"
#include "Factories/QuadFactory.h"
#include "Logic/Logic.h"
#include "SoundPlayer/SoundPlayer.h"

int last_letter_idx = 43;
const char SaveScoreState::letters[] = {
	'a', 'à', 'b', 'c', 'ç', 'd', 'e', 'è', 'é', 'f', 'g', 'h', 
	'i', 'í', 'j', 'k', 'l', 'm', 'n', 'o', 'ò', 'ó', 'p',
	'q', 'r', 's', 't', 'u', 'ú', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

const float SaveScoreState::render_initial_idx_time = 0.15f;

SaveScoreState::SaveScoreState()
: fading_in( true )
, fading_out( false )
, initials_idx( 0 )
, letters_idx( 0 )
, curr_render_initial_idx_time( 0.0f )
, render_initial_idx( true )
{
	initials[ 0 ] = 0x00;
}

SaveScoreState::~SaveScoreState() {
	printDebugString("Destroying SaveScoreState...\n");
}

void SaveScoreState::update( float deltaTime ) {
	if ( fading_in ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			fading_in = false;
	}
	else if( fading_out ) {
		if( !Global::fade.isFinished( ) || ioStatus.selectOption.becomesPressed() )
			Global::fade.update( deltaTime );
		else {
			// Save score
			Global::score_recorder.addScore( initials, Logic::getScore( ) );
			Global::score_recorder.saveScoreInfos( );

			// Go to final scores
			getState("final_scores_state")->getFocus( );
			return;
		}
	}
	else {

		if( ioStatus.walk.becomesPressedUp( ) ) {
			letters_idx = ( letters_idx == last_letter_idx ) ? 0 : letters_idx + 1;
			initials[ initials_idx ] = letters[ letters_idx ];

			SoundPlayer::get( )->playSample( "option_fixed" );
		}
		else if( ioStatus.walk.becomesPressedDown( ) ) {
			letters_idx = ( letters_idx == 0 ) ? last_letter_idx : letters_idx - 1;
			initials[ initials_idx ] = letters[ letters_idx ];

			SoundPlayer::get( )->playSample( "option_fixed" );
		}
		else if( ioStatus.start.becomesPressed( ) || ioStatus.selectOption.becomesPressed() ) {
			++initials_idx;
			SoundPlayer::get( )->playSample( "option_selected" );

			if( initials_idx == NUM_CHARS_PER_INITIALS-1 ) {
				fading_out = true;
			}
			else {
				letters_idx = 0;
				initials[ initials_idx ] = letters[ 0 ];
			}
		}

		// Actualiar el parpadeo de ENTER msg
		curr_render_initial_idx_time += deltaTime;
		if( curr_render_initial_idx_time >= render_initial_idx_time ) {
			curr_render_initial_idx_time -= render_initial_idx_time;
			render_initial_idx = !render_initial_idx;
		}
	}
}

void SaveScoreState::preRender( ) {
	activeCameraOrtho( );
	background.render( );
	activeCamera3D( );
}

void SaveScoreState::renderWorld3D( ) {
	//renderStateEnableZTest( );
	//renderStateEnableZWriting( );

	//renderStateDisableZWriting( );
	//renderStateDisableZTest( );
}

void SaveScoreState::renderWorld2D( ) {
	//renderStateEnableAlphaBlending( );
	//renderStateEnableZTest( );

	//renderStateDisableZTest( );
	//renderStateDisableAlphaBlending( );
}

void SaveScoreState::renderPostProcessed2D( ) {
}

void SaveScoreState::renderGameUI( ) {
	renderStateEnableAlphaBlending( );

	alphabet.print( 0.22f, 0.7f, 0.07f, 0.07f, "type your name"/*"escriu el teu nom"*/);

	static const float letter_width = 0.1f;
	for( size_t i=0; i<NUM_CHARS_PER_INITIALS; ++i ) {
		if( i == initials_idx && !render_initial_idx )
			continue;

		alphabet.printf( 0.4f + i*letter_width*0.75f, 0.5f, letter_width, 0.1f, "%c", initials[ i ] );
	}

	renderStateDisableAlphaBlending( );
}

void SaveScoreState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Save Score Status");
#endif

	// Render fade in
	if( fading_in ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeIn(0.5f);
		renderStateDisableAlphaBlending( );
	}
	// Render fade out
	else if( fading_out ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeOut(0.5f);
		renderStateDisableAlphaBlending( );
	}
}

void SaveScoreState::onLoad( ) {
	fading_in = true;
	fading_out = false;

	initials_idx = 0;
	letters_idx  = 0;

	initials[ 0 ] = letters[ 0 ];
	for( size_t i=1; i<NUM_CHARS_PER_INITIALS-1; ++i )
		initials[ i ] = '-';
	initials[ NUM_CHARS_PER_INITIALS-1 ] = '\0';
}

void SaveScoreState::onUnload( ) {
}

void SaveScoreState::loadGameState() {
	// Background
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		background.SETMATERIAL("top_score_pan.dds", "shader.fx", "dummy_tech");
	else
		background.SETMATERIAL("top_score.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	background.setMesh(mesh);

	// Alphabet
	alphabet.load( );
}
