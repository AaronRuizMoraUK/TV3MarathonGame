#include "IllnessState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Bass/SoundPlayer.h"
#include "Camera/ICamera.h"
#include "Source/Game.h"
#include "Factories/QuadFactory.h"

//#define THIS_IS_DEMO

const float IllnessState::print_enter_time = 0.2f;

IllnessState::IllnessState()
: next_instruction( InstructionsState::NONE )
, fading_in( true )
, fading_out( false )
, curr_print_enter_time( 0.0f )
, print_enter( true )
{
}

IllnessState::~IllnessState() {
	printDebugString("Destroying IllnessState...\n");
}

void IllnessState::update( float deltaTime ) {
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
			fading_out = false;

			// Go to instruction state
			InstructionsState *instructions_state = (InstructionsState *) getState("instructions_state");
			instructions_state->instruction = next_instruction;
			instructions_state->getFocus();
		}
	}
	else {
		if( ioStatus.start.becomesPressed( ) || ioStatus.selectOption.becomesPressed() ) {
			SoundPlayer::get( )->playSample( "option_selected" );
			fading_out = true;
		}
	}

	// Actualiar el parpadeo de ENTER msg
	curr_print_enter_time += deltaTime;
	if( curr_print_enter_time >= print_enter_time ) {
		curr_print_enter_time = print_enter_time - curr_print_enter_time;
		print_enter = !print_enter;
	}
}

void IllnessState::preRender( ) {
	activeCameraOrtho( );
	background.render( );
	activeCamera3D( );
}

void IllnessState::renderWorld3D( ) {
	//renderStateEnableZTest( );
	//renderStateEnableZWriting( );

	//renderStateDisableZWriting( );
	//renderStateDisableZTest( );
}

void IllnessState::renderWorld2D( ) {
	//renderStateEnableAlphaBlending( );
	//renderStateEnableZTest( );

	//renderStateDisableZTest( );
	//renderStateDisableAlphaBlending( );
}

void IllnessState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	//alphabet.print(0.43f, 0.665f, 0.08f, 0.08f, "SCORE");
	if( print_enter )
		alphabet.print( 0.63f, 0.1f, 0.03f, 0.03f, "ENTRAR - CONTINUAR" );

	renderStateDisableAlphaBlending( );
}

void IllnessState::renderGameUI( ) {
}

void IllnessState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Illness Status");
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

void IllnessState::selectIllnessBackground( ) {
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	bool  is_pan = aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f;

	switch( next_instruction ) {
		// After win FIRST level
		case InstructionsState::INIT_SECOND_LEVEL:
#ifdef THIS_IS_DEMO
			next_instruction = InstructionsState::GO_TO_MENU;
			background.CHANGETEXTURE( ( is_pan ) ? "continue_pan.dds" : "continue.dds" );
#else
			background.CHANGETEXTURE( ( is_pan ) ? "illness_level1_pan.dds" : "illness_level1.dds" );
#endif
			break;

		// After win SECOND level
		case InstructionsState::INIT_THIRD_LEVEL:
			background.CHANGETEXTURE( ( is_pan ) ? "illness_level2_pan.dds" : "illness_level2.dds" );
			break;

		// After win THIRD level
		case InstructionsState::INIT_FOURTH_LEVEL:
			background.CHANGETEXTURE( ( is_pan ) ? "illness_level3_pan.dds" : "illness_level3.dds" );
			break;

		// After win FOURH level
		case InstructionsState::GAME_WON:
			background.CHANGETEXTURE( ( is_pan ) ? "illness_level4_pan.dds" : "illness_level4.dds" );
			break;

		default:
			assert( !"This should not happend" );
			return;
	}
}

void IllnessState::onLoad( ) {
	assert( next_instruction != InstructionsState::NONE );
	fading_in = true;
	fading_out = false;

	SoundPlayer::get( )->playStream( "illness" );

	curr_print_enter_time = 0.0f;
	print_enter = true;

	selectIllnessBackground( );
}

void IllnessState::onUnload( ) {
	next_instruction = InstructionsState::NONE;
	SoundPlayer::get( )->stopStream( true );
}

void IllnessState::loadGameState() {
	// Background
	background.SETMATERIAL("score_recount.dds", "shader.fx", "dummy_tech");
	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	background.setMesh(mesh);

	// Alphabet
	alphabet.load( );
}
