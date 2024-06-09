#include <cassert>
#include "OptionsState.h"
#include "MenuState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Game/Game.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Factories/EffectFactory.h"
#include "Camera/ICamera.h"
#include "Factories/QuadFactory.h"
#include <fstream>

#define STEP_VOLUME 5

const D3DXVECTOR3 OptionsState::onExitPosition( -0.80f, 0.6f, 0.0f );
const D3DXVECTOR3 OptionsState::onResolutionPosition( -0.80f, 0.4f, 0.0f );
const D3DXVECTOR3 OptionsState::onQualityPosition( -0.80f, 0.2f, 0.0f );
const D3DXVECTOR3 OptionsState::onMusicPosition( -0.80f, 0.0f, 0.0f );
const D3DXVECTOR3 OptionsState::onSFXPosition( -0.80f, -0.2f, 0.0f );
const D3DXVECTOR3 OptionsState::onButtonPosition( -0.80f, -0.4f, 0.0f );

OptionsState::OptionsState()
: enter_options(true)
, exit_options(false)
, warning_changes(false)
, pressAButtonNow( false )
, optionPosition( EXIT )
, quality( HIGH )
, buttonsOptionPosition( KICK_KEY )
, isChangingSetting( false )
, outputStringStream( "" )
{
}

OptionsState::~OptionsState() {
	printDebugString("Destroying OptionsState...\n");
}

void OptionsState::update( float deltaTime ) {

	if ( enter_options ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			enter_options = false;
	}
	else if( exit_options ) {
		if( !Global::fade.isFinished( ) ) {
			applyChanges( );
			Global::fade.update( deltaTime );
		}
		else {
			// Go to menu state, but telling it that not make its onLoad
			MenuState * menuState = (MenuState *) getState( "menu_state" );
			menuState->doOnLoad = false;
			menuState->getFocus( );
		}
	}
	else if( warning_changes ) {
		if( ioStatus.start.becomesPressed( )
			|| ioStatus.selectOption.becomesPressed( ) )
			exit_options = true;
	}
	else {

		if( !isChangingSetting ) {
			// We are in the first level chossing an option

			if( (ioStatus.exitOptions.becomesPressed( ) 
				|| ioStatus.start.becomesPressed( )
				|| ioStatus.selectOption.becomesPressed( )) &&
				optionPosition == EXIT ) {
					// If resolution changes show warning msg
					if( currentOptions.resolution != Game::configOptions.resolution )
						warning_changes = true;
					else
						exit_options = true;
			}
			else if( ioStatus.exitOptions.becomesPressed( ) ) {
				optionPosition = EXIT;
				arrow.loc.setPosition( onExitPosition );
			}
			else if( ioStatus.start.becomesPressed( )
				|| ioStatus.selectOption.becomesPressed( ) ) {
					isChangingSetting = true; // We pass to change setting
			}
			else if (  ioStatus.walk.becomesPressedUp() && optionPosition != EXIT ) {
				optionPosition = (OptionPosition)((optionPosition+NUM_OPTION_POSITIONS-1)%NUM_OPTION_POSITIONS);
				if( optionPosition == EXIT )
					arrow.loc.setPosition( onExitPosition );
				else if( optionPosition == RESOLUTION )
					arrow.loc.setPosition( onResolutionPosition );
				else if( optionPosition == QUALITY )
					arrow.loc.setPosition( onQualityPosition );
				else if( optionPosition == MUSIC )
					arrow.loc.setPosition( onMusicPosition );
				else if( optionPosition == SFX )
					arrow.loc.setPosition( onSFXPosition );
				else if( optionPosition == BUTTONS )
					arrow.loc.setPosition( onButtonPosition );
			}
			else if (  ioStatus.walk.becomesPressedDown() && optionPosition != BUTTONS ) {
				optionPosition = (OptionPosition)((optionPosition+1)%NUM_OPTION_POSITIONS);
				if( optionPosition == EXIT )
					arrow.loc.setPosition( onExitPosition );
				else if( optionPosition == RESOLUTION )
					arrow.loc.setPosition( onResolutionPosition );
				else if( optionPosition == QUALITY )
					arrow.loc.setPosition( onQualityPosition );
				else if( optionPosition == MUSIC )
					arrow.loc.setPosition( onMusicPosition );
				else if( optionPosition == SFX )
					arrow.loc.setPosition( onSFXPosition );
				else if( optionPosition == BUTTONS )
					arrow.loc.setPosition( onButtonPosition );
			}

		}
		else {
			if( pressAButtonNow ) {
				// Update press button fade
				pressButtonFade.update( deltaTime );

				// We have to press a button or press ESC
				updateButtonsSettings( );
			}
			else {
				// We are in any options trying to change its value
				updateSetting( );
			}
		}

	}
}

void OptionsState::updateSetting( ) {

	// We pass to the first level again
	if( ioStatus.exitOptions.becomesPressed( ) ) {
		isChangingSetting = false;
		return;
	}

	// Set the positions and rotation of the selection quad
	D3DXMATRIX scl;
	D3DXMatrixScaling( &scl, 1.0f, 1.0f/Game::configOptions.resolution.aspect_ratio, 1.0f );
	D3DXMATRIX trf;
	D3DXMatrixRotationZ( &trf, 3.0f*Game::shader_time );
	D3DXMatrixMultiply( &trf, &scl, &trf );
	D3DXMatrixScaling( &scl, 1.0f, Game::configOptions.resolution.aspect_ratio, 1.0f );
	D3DXMatrixMultiply( &trf, &trf, &scl );
	selectionQuad.loc.setTransform( trf );
	if( optionPosition != BUTTONS )
		selectionQuad.loc.setPosition( arrow.loc.getPosition( ) + D3DXVECTOR3(0.93f, 0.0f, 0.0f) );
	else
		udpatePositionInButtons( );

	// Update Options
	if( optionPosition == RESOLUTION ) {
		updateResolution( );
	}
	else if( optionPosition == QUALITY ) {
		updateQuality( );
	}
	else if( optionPosition == MUSIC ) {
		updateMusic( );
	}
	else if( optionPosition == SFX ) {
		updateSFX( );
	}
	else if( optionPosition == BUTTONS ) {
		// When player press start on a button, pass to "press a button now" mode
		if( ioStatus.enterOptions.becomesPressed() ) {
			pressAButtonNow = true;
			return;
		}

		// Necessary for fixing problem in XInputGetKeystroke function on XBOX360GAMEPAD in IOStatus
		std::string fix;
		ioStatus.detectButtonOnInput( XBOX360GAMEPAD, fix );
	}
	else {
		assert( !"Option position do not supported in OptionState" );
		return;
	}
}

void OptionsState::updateResolution( ) {
	// We pass to the first level again
	if( ioStatus.start.becomesPressed( )
		|| ioStatus.selectOption.becomesPressed( ) ) {
			isChangingSetting = false;
			return;
	}
	else if( ioStatus.walk.becomesPressedDown() && resolutionsIt != Game::allowedResolutions.begin() ) {
		--resolutionsIt;
		currentOptions.resolution = (*resolutionsIt);
	}
	else if( ioStatus.walk.becomesPressedUp() &&
		resolutionsIt != --Game::allowedResolutions.end() &&
		resolutionsIt != Game::allowedResolutions.end() ) {
			++resolutionsIt;
			currentOptions.resolution = (*resolutionsIt);
	}
}

void OptionsState::updateQuality( ) {
	// We pass to the first level again
	if( ioStatus.start.becomesPressed( )
		|| ioStatus.selectOption.becomesPressed( ) ) {
			isChangingSetting = false;
			return;
	}
	else if( ioStatus.walk.becomesPressedDown() && quality != LOW ) {
		quality = (Quality) (quality - 1);
		currentOptions.quality = getQualityString( quality );
	}
	else if( ioStatus.walk.becomesPressedUp() && quality != HIGH ) {
		quality = (Quality) (quality + 1);
		currentOptions.quality = getQualityString( quality );
	}
}

void OptionsState::updateMusic( ) {
	// We pass to the first level again
	if( ioStatus.start.becomesPressed( )
		|| ioStatus.selectOption.becomesPressed( ) ) {
			isChangingSetting = false;
			return;
	}
	else if( ioStatus.walk.becomesPressedUp() ) {
		SoundPlayer *sound_player = SoundPlayer::get( );
		sound_player->setStreamsVolume( sound_player->getStreamsVolume( ) + STEP_VOLUME );
		currentOptions.music = sound_player->getStreamsVolume( );
	}
	else if( ioStatus.walk.becomesPressedDown() ) {
		SoundPlayer *sound_player = SoundPlayer::get( );
		sound_player->setStreamsVolume( sound_player->getStreamsVolume( ) - STEP_VOLUME );
		currentOptions.music = sound_player->getStreamsVolume( );
	}
}

void OptionsState::updateSFX( ) {
	// We pass to the first level again

	if( ioStatus.start.becomesPressed( )
		|| ioStatus.selectOption.becomesPressed( ) ) {
			isChangingSetting = false;
			return;
	}
	else if( ioStatus.walk.becomesPressedUp() ) {		
		SoundPlayer *sound_player = SoundPlayer::get( );
		sound_player->setSamplesVolume( sound_player->getSamplesVolume( ) + STEP_VOLUME );
		sound_player->playSample( "dopamine_hit_received_1" );
		currentOptions.sfx = sound_player->getSamplesVolume( );
	}
	else if( ioStatus.walk.becomesPressedDown() ) {		
		SoundPlayer *sound_player = SoundPlayer::get( );
		sound_player->setSamplesVolume( sound_player->getSamplesVolume( ) - STEP_VOLUME );
		sound_player->playSample( "dopamine_hit_received_1" );
		currentOptions.sfx = sound_player->getSamplesVolume( );
	}
}

void OptionsState::udpatePositionInButtons( ) {

	int x = buttonsOptionPosition % 3;
	int y = buttonsOptionPosition / 3;

	if( ioStatus.walk.becomesPressedUp() ) {
		if( buttonsOptionPosition == PAUSE_360 ) {
			buttonsOptionPosition = KICK_360;
		}
		// If we are not in the first line
		else if( y != 0 ) {
			buttonsOptionPosition = (ButtonsOptionPosition) (buttonsOptionPosition - 3);
		}
	}
	else if( ioStatus.walk.becomesPressedDown() ) {
		if( buttonsOptionPosition == KICK_360 ) {
			buttonsOptionPosition = PAUSE_360;
		}
		// If we are not in the last line
		else if( y != 2 && buttonsOptionPosition != WALK_KEY ) {
			buttonsOptionPosition = (ButtonsOptionPosition) (buttonsOptionPosition + 3);
		}
	}
	else if( ioStatus.walk.becomesPressedRight() ) {
		// If we are not in the right line
		if( x != 2 && buttonsOptionPosition != WALK_PAD ) {
			buttonsOptionPosition = (ButtonsOptionPosition) (buttonsOptionPosition + 1);
		}
	}
	else if( ioStatus.walk.becomesPressedLeft() ) {
		// If we are not in the left line
		if( x != 0 && buttonsOptionPosition != PAUSE_PAD ) {
			buttonsOptionPosition = (ButtonsOptionPosition) (buttonsOptionPosition - 1);
		}
	}

	D3DXVECTOR3 displacement( 0.31f*x, -0.11f*y, 0.0f );
	if( x == 0 )
		displacement.x -= 0.088f;
	selectionQuad.loc.setPosition( arrow.loc.getPosition( ) + D3DXVECTOR3(0.88f, -0.13f, 0.0f) + displacement );
}

void OptionsState::updateButtonsSettings( ) {
	// Continue rotating de selection Quad
	D3DXMATRIX scl;
	D3DXMatrixScaling( &scl, 1.0f, 1.0f/Game::configOptions.resolution.aspect_ratio, 1.0f );
	D3DXMATRIX trf;
	D3DXMatrixRotationZ( &trf, 3.0f*Game::shader_time );
	D3DXMatrixMultiply( &trf, &scl, &trf );
	D3DXMatrixScaling( &scl, 1.0f, Game::configOptions.resolution.aspect_ratio, 1.0f );
	D3DXMatrixMultiply( &trf, &trf, &scl );
	D3DXVECTOR3 prevPos = selectionQuad.loc.getPosition( );
	selectionQuad.loc.setTransform( trf );
	selectionQuad.loc.setPosition( prevPos );

	// We exit of the "press a button now" mode
	if( ioStatus.exitOptions.becomesPressed( ) ) {
		pressAButtonNow = false;
		return;
	}

	// Obtain by the current position in button what input is
	InputDevices input;
	int valuePos = buttonsOptionPosition % 3;
	if( valuePos == 0 )
		input = KEYBOARD;
	else if( valuePos == 1 )
		input = GAMEPAD;
	else if( valuePos == 2 )
		input = XBOX360GAMEPAD;
	else {
		assert( !"Position in the buttons unknown in OptionsState" );
		return;
	}

	// Detect if any button has been pressed, and if it's so, what button
	std::string buttonPressed = "";
	bool buttonDetected = ioStatus.detectButtonOnInput( input, buttonPressed );

	// If any button was pressed
	if( buttonDetected ) {
		//printfDebugString("Pushed: %s\n", buttonPressed.c_str());

		// Obtain the current action
		IOStatus::Actions action;
		int valueAct = buttonsOptionPosition / 3;
		if( valueAct == 0 )
			action = IOStatus::KICK;
		else if( valueAct == 1 )
			action = IOStatus::WALK;
		else if( valueAct == 2 )
			action = IOStatus::PAUSE;
		else {
			assert( !"Action in the buttons unknown in OptionsState" );
			return;
		}

		// Modify ioStatus and xml
		ioStatus.setButton( input, action, buttonPressed );
		ioStatus.writeXML( );

		// We exit of the "press a button now" mode
		pressAButtonNow = false;
		return;
	}

}

void OptionsState::applyChanges( ) {

	// Changes the configOptions of Game and write it in XML

	bool writeXML = false;

	// Resolution
	if( currentOptions.resolution != Game::configOptions.resolution ) {
		writeXML = true;
	}

	// Quality
	if( currentOptions.quality != Game::configOptions.quality ) {

		Game::configOptions.quality = currentOptions.quality;
		writeXML = true;

		// Change Quality
		changeQuality( );
	}

	// Volumes
	if( currentOptions.music != Game::configOptions.music ||
		currentOptions.sfx   != Game::configOptions.sfx ) {

			Game::configOptions.music = currentOptions.music;
			Game::configOptions.sfx = currentOptions.sfx;
			writeXML = true;
	}

	// Write in xml the new options
	if( writeXML ) {
		static const char *configFile = "./data/config.xml";

		// Insert the header in the string output stream
		outputStringStream << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
		outputStringStream << "<config>\n";

		// Insert the attributes in the string output stream
		xmlParseFile( configFile );

		// Insert the final in the string output stream
		outputStringStream << "</config>\n";

		// Save the string output stream to a file
		std::ofstream xmlStream;
		xmlStream.open( configFile );
		xmlStream << outputStringStream.str();
		xmlStream.close();

		// Clear the string output stream
		outputStringStream.str("");
	}
}

void OptionsState::changeQuality( ) {
	//Game::configOptions.renderTextureEnabled = (Game::configOptions.quality == "alt");

	// Reload Shaders
	EffectFactory::get( )->reload( );
}

void OptionsState::preRender( ) {
	activeCameraOrtho( );
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->background.render();
	activeCamera3D( );
}

void OptionsState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Menu State Renders
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->menuPlanet.render();

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void OptionsState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->startAction.render();
	menuState->optionsAction.render();
	menuState->creditsAction.render();
	menuState->scoresAction.render();
	menuState->exitAction.render();

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void OptionsState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->title.render();

	renderStateDisableAlphaBlending( );
}

void OptionsState::renderGameUI( ) {
	renderStateEnableAlphaBlending();

	// Render fade medium out
	if( enter_options )
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render fade in
	else if( exit_options )
		Global::fade.renderFadeIn(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Warning changes
	else if( warning_changes ) {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));

		flowerChangesUp.render( );
		flowerChangesDown.render( );

		alphabetRender.print(0.14f, 0.6f, 0.05f, 0.05f, "Resolution will change"/*"la resolució canviarà la proxima"*/);
		alphabetRender.print(0.2f, 0.55f, 0.05f, 0.05f, "when you restart the game"/*"vegada que arrenquis el joc"*/);
		alphabetRender.print(0.38f, 0.4f, 0.05f, 0.05f, "Press Enter"/*"prem entrar"*/);
	}
	// Render options
	else {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
		
		flowerUp.render( );
		flowerDown.render( );

		alphabetRender.print(0.710f, 0.95f, 0.03f, 0.031f, "enter-select"/*"entrar-seleccionar"*/);
		alphabetRender.print(0.710f, 0.91f, 0.03f, 0.031f, "      option"/*"       opció"*/);
		alphabetRender.print(0.709f, 0.87f, 0.03f, 0.031f, "    esc-exit"/*"   esc-sortir"*/);
		alphabetRender.print(0.709f, 0.83f, 0.03f, 0.031f, "      option"/*"       opció"*/);

		alphabetRender.print(0.155f, 0.8f, 0.06f, 0.06f, "Exit"/*"sortir"*/);
		alphabetRender.print(0.155f, 0.7f, 0.06f, 0.06f, "Resolution"/*"resolució"*/);
		alphabetRender.print(0.155f, 0.6f, 0.06f, 0.06f, "Quality"/*"qualitat"*/);
		alphabetRender.print(0.155f, 0.5f, 0.06f, 0.06f, "Music"/*"música"*/);
		alphabetRender.print(0.155f, 0.4f, 0.06f, 0.06f, "SFX"/*"SFX"*/);
		alphabetRender.print(0.155f, 0.3f, 0.06f, 0.06f, "Buttons"/*"botons"*/);

		if( currentOptions.resolution.rate != -1 )
			alphabetRender.printf(0.6f, 0.7f, 0.05f, 0.05f, "%dx%d:%dHz", currentOptions.resolution.width, currentOptions.resolution.height, currentOptions.resolution.rate);
		else
			alphabetRender.printf(0.6f, 0.7f, 0.05f, 0.05f, "%dx%d", currentOptions.resolution.width, currentOptions.resolution.height);
		alphabetRender.printf(0.6f, 0.6f, 0.05f, 0.05f, "%s", currentOptions.quality.c_str());
		alphabetRender.printf(0.6f, 0.5f, 0.05f, 0.05f, "%d%%", currentOptions.music);
		alphabetRender.printf(0.6f, 0.4f, 0.05f, 0.05f, "%d%%", currentOptions.sfx);

		alphabetRender.print(0.43f, 0.3f, 0.035f, 0.035f, "  Key+Mouse   PAD    XBOX360"/*"  teclat+ratolí  PAD   XBOX360"*/);
		alphabetRender.print(0.25f, 0.23f, 0.04f, 0.04f, "Kick"/*"patada"*/);
		alphabetRender.print(0.25f, 0.175f, 0.04f, 0.04f, "Walk"/*"caminar"*/);
		alphabetRender.print(0.25f, 0.12f, 0.04f, 0.04f, "Start-Pause"/*"entrar-pausa"*/);

		// Obtain the config keys
		const DigitalActionConfigKeys & kickConfigKeys = ioStatus.kick.getDigitalActionConfigKeys( );
		const DigitalActionConfigKeys & walkConfigKeys = ioStatus.walkSlow.getDigitalActionConfigKeys( );
		const DigitalActionConfigKeys & startConfigKeys = ioStatus.start.getDigitalActionConfigKeys( );

		printfKeyKeyboard( 0.23f,  "%s", kickConfigKeys.getKeyboardConfigKey( ).c_str());
		printfKeyKeyboard( 0.175f, "%s", walkConfigKeys.getKeyboardConfigKey( ).c_str());
		printfKeyKeyboard( 0.12f, "ENTER" );

		padButtonsRender.printf(0.745f, 0.23f, 0.06f, 0.06f,  "%d", kickConfigKeys.getGamePadConfigKey( ));
		padButtonsRender.printf(0.745f, 0.175f, 0.06f, 0.06f, "%d", walkConfigKeys.getGamePadConfigKey( ));
		padButtonsRender.printf(0.745f, 0.12f, 0.06f, 0.06f,  "%d", startConfigKeys.getGamePadConfigKey( ));

		xboxButtonsRender.printf(0.89f, 0.23f,  0.06f, 0.06f,  "%s", kickConfigKeys.getXBOX360GamePadConfigKey( ).c_str());
		alphabetRender.print(    0.89f, 0.175f, 0.06f, 0.06f,  "-");
		xboxButtonsRender.printf(0.89f, 0.12f,  0.06f, 0.06f, "%s", startConfigKeys.getXBOX360GamePadConfigKey( ).c_str());

		arrow.render( );
		if( isChangingSetting )
			selectionQuad.render( );

		if( pressAButtonNow ) {
			pressButtonFade.renderMediumFadeOut( 0.001f );
			alphabetRender.print(0.2f, 0.55f, 0.07f, 0.07f, "Press any button or\n  ESCAPE to exit"/*"prem qualsevol botó\n o esc per sortit"*/ );
		}
	}

	renderStateDisableAlphaBlending();
}

void OptionsState::printfKeyKeyboard( float y, const char *msg, ... ) {
	char txt[512];
	va_list ap;
	va_start(ap, msg);
	vsnprintf_s(txt, sizeof( txt ), 511, msg, ap);
	va_end (ap);

	std::string letters( txt );

	std::string::size_type lettersSize = letters.size();
	if( lettersSize == 0 ) {
		return;
		//assert( !"Letters size incorrect in OptionsState" );
	}

	const float size = 0.04f;
	float lettersMiddleWidth = size * (lettersSize-1.0f) / 4.0f;

	alphabetRender.printf(0.58f-lettersMiddleWidth, y, size, size, "%s", letters.c_str());
}

void OptionsState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Credits Status");
#endif
}

void OptionsState::onLoad( ) {
	enter_options = true;
	exit_options = false;
	warning_changes = false;
	optionPosition = EXIT;

	pressAButtonNow = false;

	buttonsOptionPosition = KICK_KEY;

	// Arrow position
	arrow.loc.setPosition( onExitPosition );

	isChangingSetting = false;

	currentOptions = Game::configOptions;

	resolutionsIt = Game::allowedResolutions.find( Game::configOptions.resolution );
	assert( !(Game::configOptions.fullScreen && resolutionsIt == Game::allowedResolutions.end()) ||
		!"Wrong current resolution in OptionsState" );

	quality = getQuality( Game::configOptions.quality );
}

void OptionsState::onUnload( ) {
}

void OptionsState::loadGameState( ) {
	// Arrow object
	const IMesh * arrowMesh = QuadFactory::get()->createQuad(0.1f, 0.1f * Game::configOptions.resolution.aspect_ratio );
	assert(arrowMesh);
	arrow.setMesh(arrowMesh);
	arrow.SETMATERIAL("selection_icon.dds", "shader.fx", "dummy_tech");

	// Selection Quad object
	const IMesh * selectionQuadMesh = QuadFactory::get()->createQuad(0.04f, 0.04f * Game::configOptions.resolution.aspect_ratio );
	assert(selectionQuadMesh);
	selectionQuad.setMesh(selectionQuadMesh);
	selectionQuad.SETMATERIAL("current_selection_icon.dds", "shader.fx", "dummy_tech");

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

	// Flower Changes down
	const IMesh * flowerChangesDownMesh = QuadFactory::get()->createQuad(0.5f, 0.125f * Game::configOptions.resolution.aspect_ratio );
	assert(flowerChangesDownMesh);
	flowerChangesDown.setMesh(flowerChangesDownMesh);
	flowerChangesDown.SETMATERIAL("flower_changes.dds", "shader.fx", "dummy_tech");
	flowerChangesDown.loc.setPosition( D3DXVECTOR3(0.3f, -0.5f, 0.0f) );

	// Flower Changes up
	flowerChangesUp = flowerChangesDown;
	D3DXMATRIX flowerChangesUptransformMatrix;
	D3DXMatrixRotationZ( &flowerChangesUptransformMatrix, D3DX_PI );
	flowerChangesUp.loc.setTransform( flowerChangesUptransformMatrix );
	flowerChangesUp.loc.setPosition( D3DXVECTOR3(-0.3f, 0.5f, 0.0f) );

	// ---------------------------------

	alphabetRender.load( );
	xboxButtonsRender.load( );
	padButtonsRender.load( );

	pressButtonFade.load( );
}

void OptionsState::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( elem == "video" ) {
		atts.put( "width", currentOptions.resolution.width );
		atts.put( "height", currentOptions.resolution.height );
		atts.put( "rate", currentOptions.resolution.rate );

		atts.writeSingle(outputStringStream, elem.c_str() );
	}
	else if ( elem == "options" ) {
		atts.put( "quality", currentOptions.quality.c_str() );
		atts.put( "music", currentOptions.music );
		atts.put( "sfx", currentOptions.sfx );

		atts.writeSingle(outputStringStream, elem.c_str() );
	}
}

std::string low = "baix";
std::string medium = "mitjà";
std::string high = "alt";
const std::string &OptionsState::getQualityString( Quality pQuality ) const {
	if( pQuality == LOW )
		return low;
	else if( pQuality == MEDIUM )
		return medium;
	else if( pQuality == HIGH )
		return high;
	else {
		assert( !"Type of quality unknown in OptionsState" );
		return high;
	}
}

OptionsState::Quality OptionsState::getQuality( const std::string &pQuality ) const {
	if( pQuality == low )
		return LOW;
	else if( pQuality == medium )
		return MEDIUM;
	else if( pQuality == high )
		return HIGH;
	else {
		assert( !"Type of quality unknown in OptionsState" );
		return HIGH;
	}
}
