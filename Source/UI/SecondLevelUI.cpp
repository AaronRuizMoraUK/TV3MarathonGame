#include <cassert>
#include "SecondLevelUI.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"
#include "Logic/SecondLevelLogic.h"
#include "Factories/QuadFactory.h"
#include "Game/Game.h"
#include "Logic/Logic.h"

const float SecondLevelUI::showWelcomeMsgTime = 4.0f;
const float SecondLevelUI::showWelcomeMsgFadeTime = 1.0f;

SecondLevelUI::SecondLevelUI( )
: welcomeMessageAlpha( 0.0f )
{
}

SecondLevelUI::~SecondLevelUI( ) {
	printDebugString("Destroying SecondLevelUI...\n");
}

void SecondLevelUI::update( float deltaTime ) {
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

void SecondLevelUI::render( ) {
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

	//

	float timer_bomba = getSecondLevelLogic().GetTimerBomba();

	if (timer_bomba > 0.0f)
	{
		bomba_picture.render();
		numbers.printf( 0.5f, 0.86f, 0.05f, 0.05f, "%02.0f", timer_bomba);
	}

	float posX = 0.095f;
	int timer_bomba_serotonina = getSecondLevelLogic().getSerotoninaRemaining();
	bomba_serotonina_picture.render();
	numbers.printf( posX, 0.88f, 0.05f, 0.05f, "%d", timer_bomba_serotonina);

	float posX2 = 0.17f;
	int timer_bomba_noradrenalina = getSecondLevelLogic().getNoradrenalinaRemaining();
	bomba_noradrenalina_picture.render();
	numbers.printf( posX2, 0.88f, 0.05f, 0.05f, "%d", timer_bomba_noradrenalina);
	

	// Show welcome message
	if( !timeoutShowWelcomeMsg.isFinished() && welcomeMessageAlpha > 0.0f )
		welcomeMessage.render( 0.7f * welcomeMessageAlpha );
}

void SecondLevelUI::showWelcomeImage( ) {
	timeoutShowWelcomeMsg.init( showWelcomeMsgTime );
	welcomeMessageAlpha = 0.0f;
}

void SecondLevelUI::load( ) {
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

	// Bomba
	// Score picture object
	const IMesh * bomba_picture_mesh = QuadFactory::get()->createQuad(0.1f, 0.1f * Game::configOptions.resolution.aspect_ratio);
	assert(bomba_picture_mesh);
	bomba_picture.setMesh(bomba_picture_mesh);
	bomba_picture.loc.setPosition( D3DXVECTOR3( 0.05f, 0.81f, 0.0f ) );
	bomba_picture.SETMATERIAL("ui_bomba.dds", "shader.fx", "dummy_tech");

	// Bomba Serotonina
	// Score picture object
	assert(bomba_picture_mesh);
	bomba_serotonina_picture.setMesh(bomba_picture_mesh);
	bomba_serotonina_picture.loc.setPosition( D3DXVECTOR3( -0.85f, 0.83f, 0.0f ) );
	bomba_serotonina_picture.SETMATERIAL("ui_bomba_serotonina.dds", "shader.fx", "dummy_tech");

	// Bomba Noradrenalina
	// Score picture object
	assert(bomba_picture_mesh);
	bomba_noradrenalina_picture.setMesh(bomba_picture_mesh);
	bomba_noradrenalina_picture.loc.setPosition( D3DXVECTOR3( -0.70f, 0.83f, 0.0f ) );
	bomba_noradrenalina_picture.SETMATERIAL("ui_bomba_noradrenalina.dds", "shader.fx", "dummy_tech");

	// Numbers
	numbers.load( );

	// Welcome message to Iuranus planet
	welcomeMessage.load( "welcome_second_level.dds" );
}

void SecondLevelUI::reset( ) {
	// Destroy ShowWelcomeMessage
	timeoutShowWelcomeMsg = Timeout( );
	welcomeMessageAlpha = 0.0f;
}
