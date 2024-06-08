#include <cassert>
#include "MenuState.h"
#include "Factories/MeshFactory.h"
#include "Source/Game.h"
#include "Factories/QuadFactory.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Bass/SoundPlayer.h"
#include "AnimationControllers/ROA/ROAObject.h"
#include "Controllers/ROAResource.h"
#include "Managers/ROAResourceManager.h"
#include "Camera/Camera3D.h"
#include "GameState/Game/InstructionsState.h"
#include "Logic/Logic.h"

MenuState::MenuState()
: doOnLoad(true)
, menuStateAction(START)
, menuState(FADING_IN)
, inPlanet(false)
, inStartAction(false)
, inOptionsAction(false)
, inCreditsAction(false)
, inScoresAction(false)
, inExitAction(false)
, inTitle(false)
{
}

MenuState::~MenuState() {
	printDebugString("Destroying MenuState...\n");

	// The animation resources of menu planet belong to MenuState, so
	// set to NULL the controller in menu planet because it won't work
	menuPlanet.setAnimationController(NULL);
}

void MenuState::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( inPlanet ) {
		createPlanet(elem, atts);
		return;
	}
	else if( inStartAction ) {
		createAction(startAction, elem, atts);
		return;
	}
	else if( inOptionsAction ) {
		createAction(optionsAction, elem, atts);
		return;
	}
	else if( inCreditsAction ) {
		createAction(creditsAction, elem, atts);
		return;
	}
	else if( inScoresAction ) {
		createAction(scoresAction, elem, atts);
		return;
	}
	else if( inExitAction ) {
		createAction(exitAction, elem, atts);
		return;
	}
	else if( inTitle ) {
		createTitle(elem, atts);
		return;
	}

	if( elem == "planet" )
		inPlanet=true;
	else if( elem == "start_action" )
		inStartAction=true;
	else if( elem == "options_action" )
		inOptionsAction=true;
	else if( elem == "credits_action" )
		inCreditsAction=true;
	else if( elem == "scores_action" )
		inScoresAction=true;
	else if( elem == "exit_action" )
		inExitAction=true;
	else if( elem == "title" )
		inTitle=true;
}

void MenuState::onEndElement (const std::string &elem) {
	if( elem == "planet" )
		inPlanet=false;
	else if( elem == "start_action" )
		inStartAction=false;
	else if( elem == "options_action" )
		inOptionsAction=false;
	else if( elem == "credits_action" )
		inCreditsAction=false;
	else if( elem == "scores_action" )
		inScoresAction=false;
	else if( elem == "exit_action" )
		inExitAction=false;
	else if( elem == "title" )
		inTitle=false;
}

void MenuState::createPlanet(const std::string &elem, MKeyValue &atts) {
	if( elem == "localization" ) {
		const std::string &position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));
		menuPlanet.loc.setTransform(matrix);
	}
	else if( elem == "mesh" ) {
		const IMesh *mesh = MeshFactory::get()->createMesh(atts["name"]);
		assert(mesh);
		menuPlanet.setMesh(mesh);
	}
	else if( elem == "material" ) {
		menuPlanet.SETMATERIAL(atts["name"], atts["shader"], atts["technique"]); // VERY VERY OPTIONAL!!
	}
}

void MenuState::createAction(Object &action, const std::string &elem, MKeyValue &atts) {
	if( elem == "localization" ) {
		const std::string &position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));
		action.loc.setParentLocation( &menuPlanet.loc );
		action.loc.setLocalTransform( matrix );
		action.loc.updateTransformWithParent();
	}
	else if( elem == "mesh" ) {
		const IMesh *mesh = MeshFactory::get()->createMesh( atts["name"] );
		assert(mesh);
		action.setMesh(mesh);
	}
	else if( elem == "material" ) {
		action.SETMATERIAL(atts["name"], atts["shader"], atts["technique"]); // VERY VERY OPTIONAL!!
	}
}

void MenuState::createTitle(const std::string &elem, MKeyValue &atts) {
	if( elem == "localization" ) {
		const std::string &position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));
		title.loc.setTransform(matrix);

		// Now the mesh
		const IMesh * mesh = NULL;
		float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
		if( aspect_ratio == 4.0f/3.0f || aspect_ratio == 5.0f/4.0f )
			mesh = QuadFactory::get()->createQuad(0.5f, 0.5f * aspect_ratio);
		else
			mesh = QuadFactory::get()->createQuad(0.4f, 0.4f * aspect_ratio);
		assert(mesh);
		title.setMesh(mesh);
	}
	else if( elem == "material" ) {
		title.SETMATERIAL(atts["name"], atts["shader"], atts["technique"]); // VERY VERY OPTIONAL!!
	}
}

void MenuState::update( float deltaTime ) {

	switch( menuState ) {
		case FADING_IN:
			if( !Global::fade.isFinished( ) )
				Global::fade.update( deltaTime );
			else
				menuState = CHOOSING_ACTION;
			break;

		case CHOOSING_ACTION: {
#ifndef NDEBUG
			// PARTICLE EDITOR
			if( isPushed( 'P' ) && menuPlanet.getAnimationController()==NULL ) {
				getState( "particle_state" )->getFocus( );
				return;
			}
#endif

			if( (ioStatus.start.becomesPressed() 
				|| ioStatus.selectOption.becomesPressed())
				&& menuPlanet.getAnimationController()==NULL ) {
					SoundPlayer::get( )->playSample( "option_selected" );

					// If Play go to init iuranus
					if( menuStateAction==START ) {
						menuState = FADING_OUT;
						return;
					}
					// If Options go to options
					if( menuStateAction==OPTIONS ) {
						getState("options_state")->getFocus();
						return;
					}
					// If Credits go to credits
					if( menuStateAction==CREDITS ) {
						getState("credits_state")->getFocus();
						return;
					}
					// If Scores go to scores
					if( menuStateAction==SCORES ) {
						getState("show_scores_state")->getFocus();
						return;
					}
					// If Exit exit game
					else if( menuStateAction==EXIT ) {
						Game::exitGame( );
						return;
					}
					else {
						assert( !"Action not exists in MenuState" );
						return;
					}
			}

			// Update menu planet
			updateMenuPlanet(deltaTime);
			break; }

		case FADING_OUT: {
			if( !Global::fade.isFinished( ) ) 
				Global::fade.update( deltaTime );
			else {
				// Init logic game
				Logic::initGame( );

				// Go to instructions
				InstructionsState *instructions_state = (InstructionsState *) getState("instructions_state");
				instructions_state->instruction = InstructionsState::INIT_FIRST_LEVEL;
				instructions_state->getFocus();
			}
			break; }

		default:
			assert( !"This should not happend" );
			return;
	}

	// Update cartels
	startAction.loc.updateTransformWithParent();
	optionsAction.loc.updateTransformWithParent();
	creditsAction.loc.updateTransformWithParent();
	scoresAction.loc.updateTransformWithParent();
	exitAction.loc.updateTransformWithParent();
}

void MenuState::updateMenuPlanet(float deltaTime) {
	IAnimationController *menuPlanetAnimationController = menuPlanet.getAnimationController();

	if( menuPlanetAnimationController != NULL ) {

		// If animation is finished, set menu planet animation to NULL
		if( menuPlanetAnimationController->isFinished() ) {
			SoundPlayer::get( )->playSample( "option_fixed" );
			menuPlanet.setAnimationController(NULL);
		}
		// If animation is not finished, update it
		else
			menuPlanet.updateAnimation(deltaTime);
	}
	else {
		// Check player pushed UP
		if( ioStatus.walk.becomesPressedDown() || ioStatus.walk.becomesPressedLeft() ) {
			SoundPlayer::get( )->playSample( "option_movement" );

			const ROAResource *roa_resource = ROAResourceManager::get( )->getResource( "menu_next.xml" );
			assert( roa_resource );
			ROAObject *roa = new ROAObject( &roa_resource->rigidAnimationResource, menuPlanet.loc.getPosition(), menuPlanet.loc.getRotation() );
			assert( roa );

			menuPlanet.setAnimationController( roa );
			menuPlanet.updateAnimation(deltaTime);

			menuStateAction = (MenuStateAction)((menuStateAction+NUM_MENU_STATE_ACTIONS-1)%NUM_MENU_STATE_ACTIONS);
		}
		// Check player pushed DOWN
		else if( ioStatus.walk.becomesPressedUp() || ioStatus.walk.becomesPressedRight() ) {
			SoundPlayer::get( )->playSample( "option_movement" );

			const ROAResource *roa_resource = ROAResourceManager::get( )->getResource( "menu_previous.xml" );
			assert( roa_resource );
			ROAObject *roa = new ROAObject( &roa_resource->rigidAnimationResource, menuPlanet.loc.getPosition(), menuPlanet.loc.getRotation() );
			assert( roa );

			menuPlanet.setAnimationController( roa );
			menuPlanet.updateAnimation(deltaTime);

			menuStateAction = (MenuStateAction)((menuStateAction+1)%NUM_MENU_STATE_ACTIONS);
		}
	}
}

void MenuState::preRender( ) {
	activeCameraOrtho( );
	background.render( );
	activeCamera3D( );
}

void MenuState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	menuPlanet.render();

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void MenuState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	startAction.render();
	optionsAction.render();
	creditsAction.render();
	scoresAction.render();
	exitAction.render();

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void MenuState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	title.render();

	renderStateDisableAlphaBlending( );
}

void MenuState::renderGameUI( ) {
}

void MenuState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += printf(0, py, 0xffff0000, "Menu Status: %d", menuStateAction);

	//const D3DXVECTOR3 &cameraPosition = Global::camera->getPosition();
	//py += printf(0, py, 0xffff0000, "Camera Position: (%1.2f,%1.2f,%1.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	//py += printf(0, py, 0xffff0000, "'G' Render Texture %s", Global::enabledRenderTexture?"YES":"NO");
#endif

	if( menuState == FADING_IN ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeIn( 0.5f );
		renderStateDisableAlphaBlending( );
	}
	else if( menuState == FADING_OUT ) {
		renderStateEnableAlphaBlending( );
		Global::fade.renderFadeOut( 0.5f );
		renderStateDisableAlphaBlending( );
	}
}

void MenuState::onLoad( ) {

	if( doOnLoad ) {
		SoundPlayer::get( )->stopAll( );
		SoundPlayer::get( )->playStream( "menu_background" );

		// Set the camera in init menu position
		D3DXVECTOR3 cameraInitialPosition(419.05f, 20.64f, 394.65f);
		D3DXVECTOR3 cameraLookAt( menuPlanet.loc.getPosition()+D3DXVECTOR3(0.0f,5.0f,4.0f) );
		getCamera3D( ).setUpVector( D3DXVECTOR3(0,1,0) );
		getCamera3D( ).lookAt( cameraInitialPosition, cameraLookAt );

		menuState = FADING_IN;
	}
	else
		doOnLoad = true;
}

void MenuState::onUnload( ) {
}

void MenuState::loadGameState() {
	const std::string &pathToXml = Global::data_locator.getPath("xml");
	bool ok = xmlParseFile( pathToXml + "menu.xml");
	assert(ok);

	const IMesh * background_quad = QuadFactory::get()->createQuad(1.0f, 1.0f);
	assert( background_quad );
	background.setMesh( background_quad );
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio == 4.0f/3.0f || aspect_ratio == 5.0f/4.0f )
		background.SETMATERIAL("menu_screen.dds", "shader.fx", "dummy_tech");
	else
		background.SETMATERIAL("menu_screen_pan.dds", "shader.fx", "dummy_tech");
}
