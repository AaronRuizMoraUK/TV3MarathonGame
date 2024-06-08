#include <cassert>
#include "FirstLevelUI.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"
#include "Factories/QuadFactory.h"
#include "Game/Game.h"
#include "Logic/Logic.h"
#include "Logic/FirstLevelLogic.h"
#include "IO/IOStatus.h"

//#define TEST_BAR

const D3DXVECTOR3 FirstLevelUI::start_bar_pos( -0.9f, 0.83f, 0.0f );
const float FirstLevelUI::dopamina_bar_width = 0.55f;

const float FirstLevelUI::showWelcomeMsgTime = 4.0f;
const float FirstLevelUI::showWelcomeMsgFadeTime = 1.0f;

FirstLevelUI::FirstLevelUI( )
: dopamina_bar_pivot_pos(0,0,0)
, welcomeMessageAlpha( 0.0f )
, test_perc( 0.0f )
{
}

FirstLevelUI::~FirstLevelUI( ) {
	printDebugString("Destroying FirstLevelUI...\n");
}

void FirstLevelUI::update( float deltaTime ) {
	// Update dopamina bar pivot
#ifdef TEST_BAR
	if( ioStatus.walk.becomesPressedUp( ) ) {
		test_perc += 0.05f;
		if( test_perc > 1.0f )
			test_perc = 1.0f;
	}
	else if( ioStatus.walk.becomesPressedDown( ) ) {
		test_perc -= 0.05f;
		if( test_perc < 0.0f )
			test_perc = 0.0f;
	}
	else if( ioStatus.walk.becomesPressedRight( ) ) {
		test_perc += 0.01f;
		if( test_perc > 1.0f )
			test_perc = 1.0f;
	}
	else if( ioStatus.walk.becomesPressedLeft( ) ) {
		test_perc -= 0.01f;
		if( test_perc < 0.0f )
			test_perc = 0.0f;
	}
	dopamina_bar_pivot_pos.x = start_bar_pos.x + test_perc*dopamina_bar_width;
	dopamina_bar_pivot.loc.setPosition( dopamina_bar_pivot_pos );
#else
	D3DXVECTOR3 new_pivot_pos;
	float &x0 = dopamina_bar_pivot_pos.x;
	float  x1 = getXPivot( );
	if( x0 != x1 ) {
		float step = deltaTime * 0.05f;
		if( x0 > x1 )
			step = -step;
		if( (x0 > x1 && (x0+step) < x1) || (x0 < x1 && (x0+step) > x1) )
			step = x1 - x0;
		x0 += step;
		dopamina_bar_pivot.loc.setPosition( dopamina_bar_pivot_pos );
	}
#endif

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

void FirstLevelUI::render( ) {
#ifdef TEST_BAR
	printf( 0, 110, 0xffffffff, "Per %0.3f", test_perc*100.0f );
#endif

	// Dopamina bar
	dopamina_bar.render( );
	dopamina_bar_pivot.render( );

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

	// Show welcome message
	if( !timeoutShowWelcomeMsg.isFinished() && welcomeMessageAlpha > 0.0f )
		welcomeMessage.render( 0.7f * welcomeMessageAlpha );
}

void FirstLevelUI::showWelcomeImage( ) {
	timeoutShowWelcomeMsg.init( showWelcomeMsgTime );
	welcomeMessageAlpha = 0.0f;
}

void FirstLevelUI::load( ) {
	// Domina bar picture object
	const IMesh * dopamina_bar_mesh = QuadFactory::get()->createQuad(0.32f, 0.32f * Game::configOptions.resolution.aspect_ratio);
	assert(dopamina_bar_mesh);
	dopamina_bar.setMesh(dopamina_bar_mesh);
	dopamina_bar.loc.setPosition( D3DXVECTOR3( -0.62f, 0.80f, 0.0f ) );
	dopamina_bar.SETMATERIAL("dopamina_bar.dds", "shader.fx", "dummy_tech");

	// Domina bar pivot picture object
	const IMesh * dopamina_bar_pivot_mesh = QuadFactory::get()->createQuad(0.069f, 0.069f * Game::configOptions.resolution.aspect_ratio);
	assert(dopamina_bar_pivot_mesh);
	dopamina_bar_pivot.setMesh(dopamina_bar_pivot_mesh);
	dopamina_bar_pivot_pos = start_bar_pos;
	dopamina_bar_pivot.loc.setPosition( dopamina_bar_pivot_pos );
	dopamina_bar_pivot.SETMATERIAL("dopamina_bar_pivot.dds", "shader.fx", "dummy_tech");

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

	// Numbers
	numbers.load( );

	// Welcome message to Iuranus planet
	welcomeMessage.load( "welcome_first_level.dds" );
}

void FirstLevelUI::reset( ) {
	// Reset Dopamina Bar Pivot
	dopamina_bar_pivot_pos.x = getXPivot( );
	dopamina_bar_pivot.loc.setPosition( dopamina_bar_pivot_pos );

	// Destroy ShowWelcomeMessage
	timeoutShowWelcomeMsg = Timeout( );
	welcomeMessageAlpha = 0.0f;
}

float FirstLevelUI::getXPivot( ) {
	float dopamina_excess = getFirstLevelLogic( ).getDopaminaExcess( );
	float x_pivot = start_bar_pos.x + dopamina_excess * dopamina_bar_width;
	return x_pivot;
}
