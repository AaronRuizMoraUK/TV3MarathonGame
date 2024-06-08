#include <cassert>
#include "ThirdLevelUI.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"
#include "Logic/ThirdLevelLogic.h"
#include "Factories/QuadFactory.h"
#include "Game/Game.h"
#include "Logic/Logic.h"

const float ThirdLevelUI::showWelcomeMsgTime = 4.0f;
const float ThirdLevelUI::showWelcomeMsgFadeTime = 1.0f;

ThirdLevelUI::ThirdLevelUI( )
: welcomeMessageAlpha( 0.0f )
{
}

ThirdLevelUI::~ThirdLevelUI( ) {
	printDebugString("Destroying ThirdLevelUI...\n");
}

void ThirdLevelUI::update( float deltaTime ) {
	// Update UI

	// Update welcome message time
	if( !timeoutShowWelcomeMsg.isFinished() ) {
		timeoutShowWelcomeMsg.update( deltaTime );

		float timeElapsed = timeoutShowWelcomeMsg.getElapsedTime();

		if( timeElapsed >= 0.0f &&
			timeElapsed <= showWelcomeMsgTime - showWelcomeMsgFadeTime )
		{
			assert( showWelcomeMsgFadeTime != 0.0f );
			float fade = timeElapsed / showWelcomeMsgFadeTime;
			welcomeMessageAlpha = clamp( fade, 0.0f, 1.0f );
		}
		else if ( timeElapsed > (showWelcomeMsgTime - showWelcomeMsgFadeTime) &&
			timeElapsed <= showWelcomeMsgTime)
		{
			assert( showWelcomeMsgFadeTime != 0.0f );
			float fade = (timeElapsed - (showWelcomeMsgTime - showWelcomeMsgFadeTime)) / showWelcomeMsgFadeTime;
			welcomeMessageAlpha = clamp( 1.0f-fade, 0.0f, 1.0f );
		}
	}
}

void ThirdLevelUI::render( ) {
	// Render UI Objects
	// Sync level
	sync_level_picture.render( );
	numbers.printf( 0.77f, 0.87f, 0.061f, 0.061f, "%3.0f", Logic::getPlayerSync( ) );
	
	// Lifes
	lifes_picture.render( );
	numbers.printf( 0.93f, 0.87f, 0.061f, 0.061f, "%d", Logic::getNumLifes( ) );

	// Score
	score_picture.render( );
	size_t score_points = Logic::getScore( );
	if( score_points > MAX_SCORE_VALUE_TO_RENDER ) // Aunque se grabe un numero más alto, no se muestra más de 999999
		score_points = MAX_SCORE_VALUE_TO_RENDER;
	numbers.printf( 0.052f, 0.79f, 0.048f, 0.048f, PRINTF_SCORE_CHAR, score_points );

	// Valves Status
	valve_bar.render( );
	const Valves::ValvesStatus &valves_status = getThirdLevelLogic( ).getValvesStatus( );

	static float first_x_value_grid = -0.9f;
	static float last_x_value_grid  = -0.34f;
	static float y_value_grid       = 0.85f;
	assert( valves_status.size( ) > 1 );
	float x_value_grid_separation = (last_x_value_grid-first_x_value_grid) / ((float)valves_status.size( )-1.0f);

	static float first_x_texts = 0.04f;
	static float last_x_texts  = 0.32f;
	static float y_texts       = 0.925f;
	float x_texts_separation = (last_x_texts-first_x_texts) / ((float)valves_status.size( )-1.0f);

	for( size_t i=0; i<valves_status.size( ); ++i ) {
		float x = first_x_texts + x_texts_separation*(float)i;
		float x_valve_grid = first_x_value_grid + x_value_grid_separation*(float)i;
		D3DXVECTOR3 valve_grid_position( x_valve_grid, y_value_grid, 0.0f );

		// Cuenta atras
		if( valves_status[ i ].right_status ) {
			valve_ok.loc.setPosition( valve_grid_position );
			valve_ok.render( );

			numbers.printf( x, y_texts, 0.04f, 0.04f, "%02.0f", valves_status[ i ].time_left );
		}
		// Off
		else {
			valve_no_ok.loc.setPosition( valve_grid_position );
			valve_no_ok.render( );

			alphabet.printf( x-0.005f, y_texts, 0.03f, 0.04f, "off" );
		}
		
		// Numero de valvula
		numbers.printf( x+0.01f, y_texts-0.04f, 0.04f, 0.04f, "%d", i+1 );
	}

	// Show welcome message
	if( !timeoutShowWelcomeMsg.isFinished() && welcomeMessageAlpha > 0.0f )
		welcomeMessage.render( 0.7f * welcomeMessageAlpha );
}

void ThirdLevelUI::showWelcomeImage( ) {
	timeoutShowWelcomeMsg.init( showWelcomeMsgTime );
	welcomeMessageAlpha = 0.0f;
}

void ThirdLevelUI::load( ) {
	// Load UI Objects
	// Sync level picture object
	const IMesh * sync_level_picture_mesh = QuadFactory::get()->createQuad(0.07f, 0.07f * Game::configOptions.resolution.aspect_ratio);
	assert(sync_level_picture_mesh);
	sync_level_picture.setMesh(sync_level_picture_mesh);
	sync_level_picture.loc.setPosition( D3DXVECTOR3( 0.69f, 0.83f, 0.0f ) );
	sync_level_picture.SETMATERIAL("sync_level.dds", "shader.fx", "dummy_tech");

	// Lifes picture object
	const IMesh * lifes_picture_mesh = QuadFactory::get()->createQuad(0.07f, 0.07f * Game::configOptions.resolution.aspect_ratio);
	assert(lifes_picture_mesh);
	lifes_picture.setMesh(lifes_picture_mesh);
	lifes_picture.loc.setPosition( D3DXVECTOR3( 0.89f, 0.84f, 0.0f ) );
	lifes_picture.SETMATERIAL("lifes.dds", "shader.fx", "dummy_tech");

	// Score picture object
	const IMesh * score_picture_mesh = QuadFactory::get()->createQuad(0.19f, 0.19f * Game::configOptions.resolution.aspect_ratio);
	assert(score_picture_mesh);
	score_picture.setMesh(score_picture_mesh);
	score_picture.loc.setPosition( D3DXVECTOR3( -0.78f, 0.60f, 0.0f ) );
	score_picture.SETMATERIAL("score.dds", "shader.fx", "dummy_tech");

	// Numbers and letters
	numbers.load( );
	alphabet.load( );

	// Valves UI
	const IMesh * valve_bar_mesh = QuadFactory::get()->createQuad(0.35f, 0.55f * Game::configOptions.resolution.aspect_ratio);
	assert(valve_bar_mesh);
	valve_bar.setMesh(valve_bar_mesh);
	valve_bar.loc.setPosition( D3DXVECTOR3( -0.62f, 0.85f, 0.0f ) );
	valve_bar.SETMATERIAL("gui_valve_bar.dds", "shader.fx", "dummy_tech");

	const IMesh * valve_no_ok_mesh = QuadFactory::get()->createQuad(0.06f, 0.06f * Game::configOptions.resolution.aspect_ratio);
	assert(valve_no_ok_mesh);
	valve_no_ok.setMesh(valve_no_ok_mesh);
	valve_no_ok.SETMATERIAL("gui_valve_no_ok.dds", "shader.fx", "dummy_tech");

	const IMesh * valve_ok_mesh = QuadFactory::get()->createQuad(0.06f, 0.06f * Game::configOptions.resolution.aspect_ratio);
	assert(valve_ok_mesh);
	valve_ok.setMesh(valve_ok_mesh);
	valve_ok.SETMATERIAL("gui_valve_ok.dds", "shader.fx", "dummy_tech");

	// Welcome message to Iuranus planet
	welcomeMessage.load( "welcome_third_level.dds" );
}

void ThirdLevelUI::reset( ) {
	// Destroy ShowWelcomeMessage
	timeoutShowWelcomeMsg = Timeout( );
	welcomeMessageAlpha = 0.0f;
}
