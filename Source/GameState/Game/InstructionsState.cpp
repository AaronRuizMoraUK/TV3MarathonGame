#include <cassert>
#include "InstructionsState.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"
#include "Camera/ICamera.h"
#include "Source/Game.h"
#include "Bass/SoundPlayer.h"
#include "Camera/Camera3D.h"
#include "Factories/QuadFactory.h"
#include "Object/AnimatedObject/CalAnimModel.h"
#include "Managers/CalAnimCoreModelManager.h"
#include "Factories/MeshFactory.h"
#include "Logic/Logic.h"

std::string InstructionsState::pathToXml = "";
const float InstructionsState::print_enter_time = 0.2f;

const char *InstructionsState::instructions_chars[ NUM_INSTRUCTIONS ] = {
	"NONE"
	, "GO_TO_MENU"
	, "GAME_OVER"
	, "SCORE_RECOUNT"

	, "INIT_FIRST_LEVEL"
	, "LOSE_FIRST_LEVEL"
	, "INIT_SECOND_LEVEL"
	, "LOSE_SECOND_LEVEL"
	, "INIT_THIRD_LEVEL"
	, "LOSE_THIRD_LEVEL"
	, "INIT_FOURTH_LEVEL"
	, "LOSE_FOURTH_LEVEL"
	, "GAME_WON"
};

InstructionsState::InstructionsState()
: instruction( NONE )
, IGameState()
, status( FADING_IN )
, ok_action_duration( 0.0f )
, ok_curr_time( 0.0f )
, idle_anim( "" )
, ok_anim( "" )
, curr_print_enter_time( 0.0f )
, print_enter( true )
, sound_id( 0 )
{
}

InstructionsState::~InstructionsState() {
	printDebugString("Destroying InstructionsState...\n");
}

InstructionsState::InstructionsEnun InstructionsState::getInstruction( const char *inst_text ) const {
	for( int i=0; i<NUM_INSTRUCTIONS; ++i ) {
		if( strcmp( instructions_chars[i], inst_text ) == 0 )
			return (InstructionsEnun)i;
	}

	return NONE;
}

void InstructionsState::onStartElement(const std::string &elem, MKeyValue &atts) {
	if( elem == "instruction" ) {
		const std::string &id = atts["id"];
		const std::string &inst_text = atts.getString( "text", "" );

		InstructionsEnun inst_id = getInstruction( id.c_str( ) );
		assert( inst_id >= INIT_FIRST_LEVEL ); // Valid inst_id

		instructions_texts[ inst_id ] = inst_text;
	}
	else if( elem == "comic_text_window" ) {
		// Comic text window
		comic_text_window.load( atts[ "style" ] );
	}
	else if( elem == "instructor" ) {
		// Instructor
		const std::string &cal_model = atts[ "cal_model" ];
		instructor.setCalAnimModel( cal_model.c_str( ) );
		instructor.SETMATERIAL( atts[ "texture" ], atts[ "shader" ], atts[ "technique" ] );
		idle_anim = atts["idle_anim"];
		ok_anim = atts["ok_anim"];
	}
}

void InstructionsState::update( float deltaTime ) {

	// Next states without instructions
	if( instruction == GO_TO_MENU ) {
		getState( "menu_state" )->getFocus( );
		return;
	}
	else if( instruction == GAME_OVER ) {
		getState( "game_over_state" )->getFocus( );
		return;
	}
	else if( instruction == SCORE_RECOUNT ) {
		getState( "score_recount_state" )->getFocus( );
		return;
	}

	switch( status ) {
		case FADING_IN:
			if( !Global::fade.isFinished( ) )
				Global::fade.update( deltaTime );
			else {
				// Get text of instruction
				TInstructions::const_iterator it = instructions_texts.find( instruction );
				assert( instructions_texts.end( ) != it );
				const std::string &current_instruction_text = it->second;

				// Play voice explanation
				if( comic_text_window.getComicTextType( ) == "static_text" ) {
					char instruction_sound[64];
					strcpy_s(instruction_sound, sizeof( instruction_sound ), instructions_chars[ instruction ] );
					for( size_t i=0; i<strlen( instruction_sound ); ++i ) // Minusculas
						instruction_sound[i] = tolower( instruction_sound[i] );
					assert( sound_id == 0 );
					sound_id = SoundPlayer::get( )->playSample( instruction_sound );
				}

				// Initiate the text window
				comic_text_window.initiate( current_instruction_text );
				
				// Test if all texts are right
				//TInstructions::const_iterator itTest = instructions_texts.begin( );
				//while( itTest != instructions_texts.end( ) ) {
				//	comic_text_window.initiate( itTest->second );
				//	++itTest;
				//}

				curr_print_enter_time = 0.0f;
				print_enter = true;
				status = EXPLAINING;
			}
			break;

		case EXPLAINING: {
			bool is_finished = comic_text_window.update( deltaTime );
			if( is_finished ) {
				if( ok_action_duration == 0.0f ) {
					if( comic_text_window.getComicTextType( ) == "static_text" ) {
						SoundPlayer::get( )->stopSample( sound_id ); // Stop sound
						sound_id = 0;
					}

					// TODO Dependiendo de lo que se haya hecho lanzar una accion/sonido u otro
					SoundPlayer::get( )->playSample( "player_go_brain" );

					ok_action_duration = instructor.getCalAnimModel( )->playAction( ok_anim ) + 0.2f;
					ok_curr_time = 0.0f;
				}
				else {
					ok_curr_time += deltaTime;
					if( ok_curr_time >= ok_action_duration ) {
						ok_action_duration = 0.0f;
						status = FADING_OUT;
					}
				}
			}
			else {
				// Actualiar el parpadeo de ENTER msg
				curr_print_enter_time += deltaTime;
				if( curr_print_enter_time >= print_enter_time ) {
					curr_print_enter_time = print_enter_time - curr_print_enter_time;
					print_enter = !print_enter;
				}
			}
			break; }

		case FADING_OUT: {
			if( !Global::fade.isFinished( ) )
				Global::fade.update( deltaTime );
			else {
				goToNextState( );
				return;
			}
			break; }

		default:
			assert( !"This should not happend" );
			return;
	}

	// Update of animated objects so these can be rendered
	instructor.updateAnimation( deltaTime );
}

void InstructionsState::goToNextState( ) {
	switch( instruction ) {
		case INIT_FIRST_LEVEL:
		case LOSE_FIRST_LEVEL:
			getState( "init_first_level_state" )->getFocus( );
			return;

		case INIT_SECOND_LEVEL:
		case LOSE_SECOND_LEVEL:
			getState( "init_second_level_state" )->getFocus( );
			return;

		case INIT_THIRD_LEVEL:
		case LOSE_THIRD_LEVEL:
			getState( "init_third_level_state" )->getFocus( );
			return;

		case INIT_FOURTH_LEVEL:
		case LOSE_FOURTH_LEVEL:
			getState( "init_fourth_level_state" )->getFocus( );
			return;

		case GAME_WON:
			{
			// Ir a introcudir nuestras iniciales o directamente a los creditos
			bool score_in_top = Global::score_recorder.isInsideTopScores( Logic::getScore( ) );
			if( score_in_top )
				getState( "save_score_state" )->getFocus( );
			else
				getState( "final_credits_state" )->getFocus( );
			return;
			}

		default:
			assert( !"This should not happend" );
			return;
	}
}

void InstructionsState::preRender( ) {
	activeCameraOrtho( );
	background.render( );
	activeCamera3D( );
}

void InstructionsState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	instructor.render();

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void InstructionsState::renderWorld2D( ) {
	//renderStateEnableAlphaBlending( );
	//renderStateEnableZTest( );

	//renderStateDisableZTest( );
	//renderStateDisableAlphaBlending( );
}

void InstructionsState::renderPostProcessed2D( ) {
	if( status == EXPLAINING ) {
		renderStateEnableAlphaBlending( );
		comic_text_window.render( );

		if( comic_text_window.getStatus( ) == ComicTextWindow::WRITE_TEXT ) {
			if( comic_text_window.getComicTextType( ) == "dinamic_text" ) {
				if( print_enter )
					comic_text_window.alphabet_renderer.print( 0.63f, 0.54f, 0.03f, 0.03f, "ENTRAR - CONTINUAR" );
			}
			comic_text_window.alphabet_renderer.print( 0.6f, 0.05f, 0.05f, 0.05f, "ESC - SORTIR" );
		}
		renderStateDisableAlphaBlending( );
	}
}

void InstructionsState::renderGameUI( ) {
}

void InstructionsState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Instructions Status");
	py += 10;

	py += printf(0, py, 0xffff0000, "Instruction: %s", instructions_chars[ instruction ] );
	//const D3DXVECTOR3 &camera_pos = getCamera3D( )->getPosition( );
	//py += printf(0, py, 0xffff0000, "%1.3f, %1.3f, %1.3f", camera_pos.x, camera_pos.y, camera_pos.z );
#endif

	if( status == FADING_IN ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeIn( 0.5f );
		renderStateDisableAlphaBlending( );
	}
	else if( status == FADING_OUT ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeOut( 0.5f );
		renderStateDisableAlphaBlending( );
	}
}

void InstructionsState::onLoad( ) {
	assert( instruction != NONE );
	status = FADING_IN;

	// Musica cuando ganamos
	SoundPlayer::get( )->stopStream( true );
	if( instruction == GAME_WON )
		SoundPlayer::get( )->playStream( "game_won_music" );

	// Set the camera position
	//D3DXVECTOR3 cameraInitialPosition(0.551f, 0.842f, 2.317f);
	D3DXVECTOR3 cameraInitialPosition(0.451f, 1.042f, 1.817f);
	D3DXVECTOR3 cameraLookAt( cameraInitialPosition + D3DXVECTOR3(0,0,-1.0f) );
	getCamera3D( ).setUpVector( D3DXVECTOR3(0,1,0) );
	getCamera3D( ).lookAt( cameraInitialPosition, cameraLookAt );

	instructor.getCalAnimModel( )->clearAllAnimations( );
	instructor.getCalAnimModel( )->playCycle( idle_anim, 0.0f );
}

void InstructionsState::onUnload( ) {
	instruction = NONE;
}

void InstructionsState::loadGameState() {
	// Background
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		background.SETMATERIAL("instructions_splash_pan.dds", "shader.fx", "dummy_tech");
	else
		background.SETMATERIAL("instructions_splash.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	background.setMesh(mesh);

	// Load instructor, Texts and Comic Text Window Style
	if( pathToXml.empty( ) )
		pathToXml = Global::data_locator.getPath("xml");
	bool ok = xmlParseFile( pathToXml + "instructions.xml" );
	assert( ok );
}
