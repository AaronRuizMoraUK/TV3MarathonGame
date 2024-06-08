#include <cassert>
#include "Pause.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Game/Game.h"
#include "Factories/QuadFactory.h"
#include "IO/IOStatus.h"
#include "GameState/IGameState.h"
#include "GameState/Game/FirstLevel/EndFirstLevelState.h"
#include "GameState/Game/SecondLevel/EndSecondLevelState.h"
#include "GameState/Game/ThirdLevel/EndThirdLevelState.h"
#include "GameState/Game/FourthLevel/EndFourthLevelState.h"
#include "SoundPlayer/SoundPlayer.h"

const D3DXVECTOR3 Pause::onContinuePosition( -0.1f, 0.0f, 0.0f );
D3DXVECTOR3       Pause::onBackToMenuPosition( -0.1f, -0.2f, 0.0f );

Pause::Pause( )
: choice( RESUME )
, fading_in_pause( true )
, fading_out_pause( false )
, pause_on_exit_state( "" )
, samples_volume( 0 )
{
}

Pause::~Pause( ) {
	printDebugString( "Destroying Pause...\n" );
}

void Pause::load( ) {
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;

	// Arrow object
	const IMesh * arrowMesh = QuadFactory::get()->createQuad(0.1f, 0.1f * aspect_ratio );
	assert(arrowMesh);
	arrow.setMesh(arrowMesh);
	arrow.SETMATERIAL("selection_icon.dds", "shader.fx", "dummy_tech");
	arrow.loc.setPosition( onContinuePosition );

	alphabet.load( );

	//if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
	//	onBackToMenuPosition = D3DXVECTOR3( -0.1f, -0.27f, 0.0f );

	// Flower up
	const IMesh * flowerUpMesh = QuadFactory::get()->createQuad(0.6f, 0.15f * Game::configOptions.resolution.aspect_ratio );
	assert(flowerUpMesh);
	flowerUp.setMesh(flowerUpMesh);
	flowerUp.SETMATERIAL("flower_options.dds", "shader.fx", "dummy_tech");
	flowerUp.loc.setPosition( D3DXVECTOR3(-0.3f, 0.65f, 0.0f) );

	// Flower down
	flowerDown = flowerUp;
	D3DXMATRIX flowerDowntransformMatrix;
	D3DXMatrixRotationZ( &flowerDowntransformMatrix, D3DX_PI );
	flowerDown.loc.setTransform( flowerDowntransformMatrix );
	flowerDown.loc.setPosition( D3DXVECTOR3(0.3f, -0.65f, 0.0f) );
}

void Pause::initiate( const std::string &apause_on_exit_state ) {
	choice = RESUME;
	fading_in_pause = true;
	fading_out_pause = false;
	arrow.loc.setPosition( onContinuePosition );
	pause_on_exit_state = apause_on_exit_state;

	assert( pause_on_exit_state == "end_first_level_state"
		|| pause_on_exit_state == "end_second_level_state"
		|| pause_on_exit_state == "end_third_level_state"
		|| pause_on_exit_state == "end_fourth_level_state" );
	
	//SoundPlayer::get( )->playSample( "option_selected" );

	// Guardarse el volumen y silenciar los samples
	samples_volume = SoundPlayer::get( )->getSamplesVolume( );
	SoundPlayer::get( )->setSamplesVolume( 0 );
}

bool Pause::update( float deltaTime ) {
	if ( fading_in_pause ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			fading_in_pause = false;
	}
	else if( fading_out_pause ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else {
			fading_out_pause = false;
			// Dejar el volumen de samples como estaba
			SoundPlayer::get( )->setSamplesVolume( samples_volume );
			return true; // Hemos salido del pause y ha quedado nuestra eleccion en choice
		}
	}
	else {
		if( ioStatus.start.becomesPressed( ) 
			|| ioStatus.selectOption.becomesPressed() ) {
			//SoundPlayer::get( )->playSample( "option_selected" );
			fading_out_pause = true;
		}
		else if (  ioStatus.walk.becomesPressedUp() ) {
			//SoundPlayer::get( )->playSample( "option_fixed" );
			choice = RESUME;
			arrow.loc.setPosition( onContinuePosition );
		}
		else if (  ioStatus.walk.becomesPressedDown() ) {
			//SoundPlayer::get( )->playSample( "option_fixed" );
			choice = EXIT;
			arrow.loc.setPosition( onBackToMenuPosition );
		}
	}

	return false;
}

void Pause::goToExitState( ) {
	IGameState *go_to_exit_game_state = IGameState::getState( pause_on_exit_state.c_str( ) );
	assert( go_to_exit_game_state );

	InstructionsState::InstructionsEnun *next_instruction = NULL;
	if( pause_on_exit_state == "end_first_level_state" )
		next_instruction = &((EndFirstLevelState *)go_to_exit_game_state)->next_instruction;
	else if( pause_on_exit_state == "end_second_level_state" )
		next_instruction = &((EndSecondLevelState *)go_to_exit_game_state)->next_instruction;
	else if( pause_on_exit_state == "end_third_level_state" )
		next_instruction = &((EndThirdLevelState *)go_to_exit_game_state)->next_instruction;
	else if( pause_on_exit_state == "end_fourth_level_state" )
		next_instruction = &((EndFourthLevelState *)go_to_exit_game_state)->next_instruction;
	else {
		assert( !"This should not happend" );
		return;
	}

	assert( next_instruction );
	*next_instruction = InstructionsState::GO_TO_MENU;

	go_to_exit_game_state->getFocus( );

	// Destroy pause_on_exit_state
	pause_on_exit_state = "";
}

void Pause::render( ) {
	// Render fade medium out
	if( fading_in_pause )
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render fade in
	else if( fading_out_pause )
		Global::fade.renderFadeIn(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render pause UI
	else {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));

		flowerUp.render( );
		flowerDown.render( );

		alphabet.print(0.35f, 0.7f, 0.15f, 0.15f, "PAUSA");
		alphabet.print(0.5f, 0.5f, 0.05f, 0.05f, "Continuar");
		alphabet.print(0.5f, 0.4f, 0.05f, 0.05f, "Sortir");
		arrow.render( );
	}
}
