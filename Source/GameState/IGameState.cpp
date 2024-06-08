#include <cassert>
#include "IGameState.h"
#include "Source/Game.h"
#include "Factories/QuadFactory.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"
#include "Camera/ICamera.h"

#include "Menu/MenuState.h"
#include "Menu/CreditsState.h"
#include "Menu/OptionsState.h"
#include "Menu/ShowScoresState.h"
#include "Menu/LogosState.h"

#include "Game/InstructionsState.h"
#include "Game/GameOverState.h"
#include "Game/ScoreRecountState.h"
#include "Game/IllnessState.h"
#include "Game/FinalScoresState.h"
#include "Game/FinalCreditsState.h"
#include "Game/SaveScoreState.h"

#include "Game/FirstLevel/InitFirstLevelState.h"
#include "Game/FirstLevel/FirstLevelState.h"
#include "Game/FirstLevel/EndFirstLevelState.h"
#include "Game/SecondLevel/InitSecondLevelState.h"
#include "Game/SecondLevel/SecondLevelState.h"
#include "Game/SecondLevel/EndSecondLevelState.h"
#include "Game/ThirdLevel/InitThirdLevelState.h"
#include "Game/ThirdLevel/ThirdLevelState.h"
#include "Game/ThirdLevel/EndThirdLevelState.h"
#include "Game/FourthLevel/InitFourthLevelState.h"
#include "Game/FourthLevel/FourthLevelState.h"
#include "Game/FourthLevel/EndFourthLevelState.h"

// Test Game States
#ifndef NDEBUG
#include "EmptyState.h"
#include "TestState.h"
#include "TestControllerState.h"
#include "ParticleState.h"

EmptyState           empty_state;
TestState            test_state;
TestControllerState  test_controller_state;
ParticleState        particle_state;
#endif

MenuState       menu_state;
CreditsState    credits_state;
OptionsState    options_state;
ShowScoresState show_scores_state;
LogosState      logos_state;

InstructionsState    instructions_state;
GameOverState        game_over_state;
ScoreRecountState    score_recount_state;
IllnessState         illness_state;
FinalScoresState     final_scores_state;
FinalCreditsState    final_credits_state;
SaveScoreState       save_score_state;

InitFirstLevelState  initFirstLevelState;
FirstLevelState      firstLevelState;
EndFirstLevelState   endFirstLevelState;
InitSecondLevelState initSecondLevelState;
SecondLevelState     secondLevelState;
EndSecondLevelState  endSecondLevelState;
InitThirdLevelState  initThirdLevelState;
ThirdLevelState      thirdLevelState;
EndThirdLevelState   endThirdLevelState;
InitFourthLevelState initFourthLevelState;
FourthLevelState     fourthLevelState;
EndFourthLevelState  endFourthLevelState;

IGameState * IGameState::gameStates[IGameState::numStates] = {
	&menu_state
	, &credits_state
	, &options_state
	, &show_scores_state
	, &logos_state

	, &instructions_state
	, &game_over_state
	, &score_recount_state
	, &illness_state
	, &final_scores_state
	, &final_credits_state
	, &save_score_state

	, &initFirstLevelState
	, &firstLevelState
	, &endFirstLevelState
	, &initSecondLevelState
	, &secondLevelState
	, &endSecondLevelState
	, &initThirdLevelState
	, &thirdLevelState
	, &endThirdLevelState
	, &initFourthLevelState
	, &fourthLevelState
	, &endFourthLevelState

#ifndef NDEBUG
	, &test_state
	, &test_controller_state
	, &particle_state
	, &empty_state
#endif

	, NULL
};

IGameState * IGameState::currentState = NULL;

RenderTexture IGameState::renderTexture;     
Object        IGameState::renderTextureQuad;

// Dinamic Help
DinamicHelp   IGameState::dinamicHelp;
bool          IGameState::dinamic_help_enabled = false;

// Pause
Pause					IGameState::pause;
bool          IGameState::pause_enabled = false;

bool IGameState::take_screenshot = false;

void IGameState::showLoadingScreen( ) {
	// ---------------------------------------------
	// Create loading screen object
	Object loadingSplash;

	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		loadingSplash.SETMATERIAL("loading_splash_pan.dds", "shader.fx", "dummy_tech");
	else
		loadingSplash.SETMATERIAL("loading_splash.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad(1.0f, 1.0f);
	assert(mesh);
	loadingSplash.setMesh(mesh);

	// ---------------------------------------------
	// Begin Render Frame

	// Clear the backbuffer and z-buffer to a black color
	Global::device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	Global::device->BeginScene();

	// ---------------------------------------------
	// Render Frame
	activeCameraOrtho( );

	loadingSplash.render( );

	// ---------------------------------------------
	// End Render Frame

	// End the scene
	Global::device->EndScene();

	// Present the backbuffer contents to the display
	Global::device->Present( NULL, NULL, NULL, NULL );
}

void IGameState::updateFrame( float deltaTime ) {
	// Dinamic Help
	if( dinamic_help_enabled ) {
		bool finished = dinamicHelp.update( deltaTime );
		dinamic_help_enabled = !finished;
	}
	// Pause
	else if( pause_enabled ) {
		bool finished = pause.update( deltaTime );
		pause_enabled = !finished;
		if( finished && pause.getChoice( ) == Pause::EXIT )
			pause.goToExitState( );
	}
	// Normal Update
	else
		update( deltaTime );
}

void IGameState::render( ) {

	if( Game::configOptions.renderTextureEnabled ) {
		// Render to Texture
		beginRenderTexture( );
		preRender( );

		renderWorld3D( ); 
		renderWorld2D( );

		activeCameraOrtho( );

		renderPostProcessed2D( );
		renderGameUI( );

		postRender( );
		renderDinamicHelp( );
		renderPause( );
		endRenderTexture( );

#ifndef NDEBUG
		// Take a screenshot
		if( take_screenshot ) {
			take_screenshot = false;

			char msg[20];
			static int screenshotNumber = 0;
			sprintf_s( msg, sizeof( msg ), "screenshot_%02d", screenshotNumber++ );
			renderTexture.save( msg );
		}
#endif

		// Render texture to screen
		beginRenderFrame( );
		renderFrame( );
		endRenderFrame( );

		activeCamera3D( );
	}
	else {
		beginRenderFrame( );
		preRender( );

		renderWorld3D( ); 
		renderWorld2D( );

		activeCameraOrtho( );

		renderPostProcessed2D( );
		renderGameUI( );

		postRender( );
		renderDinamicHelp( );
		renderPause( );
		endRenderFrame( );

		activeCamera3D( );
	}
}

void IGameState::showDinamicHelp( DinamicHelp::DinamicHelpDesc help ) {
	assert( !dinamic_help_enabled );
	dinamicHelp.initiate( help );
	dinamic_help_enabled = true;
}

void IGameState::showPause( const std::string &pause_on_exit_state ) {
	assert( !pause_enabled );
	pause.initiate( pause_on_exit_state );
	pause_enabled = true;
}

void IGameState::renderDinamicHelp( ) {
	// Render Dinamic Help
	if( dinamic_help_enabled ) {
		renderStateEnableAlphaBlending( );
		dinamicHelp.render( );
		renderStateDisableAlphaBlending( );
	}
}

void IGameState::renderPause( ) {
	// Render Pause
	if( pause_enabled ) {
		renderStateEnableAlphaBlending( );
		pause.render( );
		renderStateDisableAlphaBlending( );
	}
}

void IGameState::beginRenderFrame() {
	// Clear the backbuffer and z-buffer to a black color
	Global::device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	Global::device->BeginScene();
}

void IGameState::endRenderFrame() {
	// End the scene
	Global::device->EndScene();

	// Present the backbuffer contents to the display
	Global::device->Present( NULL, NULL, NULL, NULL );
}

void IGameState::beginRenderTexture() {
	renderTexture.begin( );

	// Clear the backbuffer and z-buffer to a black color
	Global::device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
}

void IGameState::endRenderTexture() {
	renderTexture.end( );
}

void IGameState::renderFrame( ) {
	renderTextureQuad.render( );
}

void IGameState::getFocus( ) {
	if( currentState )
		currentState->onUnload();

	currentState = this;
	currentState->onLoad();
}

IGameState * IGameState::getState( const char *name ) {
	for(int i=0; i<numStates; ++i) {
		IGameState * gameState = gameStates[i];
		if( gameState == NULL )
			break;
		else if( strcmp( name, gameState->getName() ) == 0 )
			return gameState;
	}

	assert(!"State does not exist");
	return NULL;
}

/*
bool IGameState::isVisible(const Object &object) {
const IMesh * mesh = object.getMesh();
assert( mesh );
const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
float radius = mesh->getBoundingSphereRadius();

D3DXVECTOR3 centerInstance = center + object.getPosition();
if( Global::frustum.isBSphereVisible(centerInstance, radius) ) 
return true;
else
return false;
}
*/

void IGameState::load() {
	// Init Render to Texture
	renderTexture.create( Game::configOptions.resolution.width, Game::configOptions.resolution.height );

	renderTextureQuad.SETMATERIAL(renderTexture.getTexture(), "shader.fx", "dummy2_tech");
	const IMesh * renderTextureMesh = QuadFactory::get()->createQuad(1.0f, 1.0f);
	assert(renderTextureMesh);
	renderTextureQuad.setMesh(renderTextureMesh);

	// Load Dinamic Help and pause
	dinamicHelp.load( );
	pause.load( );

	// Load all objects of game states
	for( int i=0; i<numStates && gameStates[i] != NULL; ++i )
		gameStates[i]->loadGameState( );
}

void IGameState::destroy() {
	renderTexture.destroy();
}
