#ifndef OPTIONS_STATE_H
#define OPTIONS_STATE_H

#include "GameState/IGameState.h"
#include "XMLParser/XMLParser.h"
#include "Util/StringRender/AlphabetRender.h"
#include "Util/StringRender/XBOXButtonsRender.h"
#include "Util/StringRender/PadButtonsRender.h"
#include "Util/Fade/Fade.h"
#include "Game/GameOptions.h"
#include <sstream>

/**
* Menu options state
*
* Resolution: Saved in xml when you exit options, applied when you restart the game
* Quality: Saved in xml and applied when you exit options
* Music: Saved in xml when you exit options, applied directly when you changes its value
* SFX: Saved in xml when you exit options, applied directly when you changes its value
* Button: Saved in xml and applied directly when you changes its value
*/

class OptionsState : public IGameState, public CXMLParser
{
protected:
	bool enter_options;
	bool exit_options;
	bool warning_changes;

	Object arrow;
	Object selectionQuad;
	CatalanAlphabetColorRender alphabetRender;
	XBOXButtonsRender xboxButtonsRender;
	PadButtonsRender padButtonsRender;

	// Buttons Fade
	Fade pressButtonFade;
	bool pressAButtonNow;

	// Flowers pictures
	Object flowerUp;
	Object flowerDown;

	// Flowers Changes pictures
	Object flowerChangesUp;
	Object flowerChangesDown;

	// Positions of the arrow
	static const D3DXVECTOR3 onExitPosition;
	static const D3DXVECTOR3 onResolutionPosition;
	static const D3DXVECTOR3 onQualityPosition;
	static const D3DXVECTOR3 onMusicPosition;
	static const D3DXVECTOR3 onSFXPosition;
	static const D3DXVECTOR3 onButtonPosition;

	// Option position
	enum OptionPosition {EXIT, RESOLUTION, QUALITY, MUSIC, SFX, BUTTONS, NUM_OPTION_POSITIONS};
	OptionPosition optionPosition;
	Resolutions::const_iterator resolutionsIt;

	// Quality
	enum Quality{LOW, MEDIUM, HIGH, NUM_QUALITIES};
	Quality quality;

	// Selection in Buttons option position
	enum ButtonsOptionPosition {//JUMP_KEY, JUMP_PAD, JUMP_360,
		KICK_KEY, KICK_PAD, KICK_360,  
		WALK_KEY, WALK_PAD, WALK_360,
		PAUSE_KEY, PAUSE_PAD, PAUSE_360,
		NUM_BUTTONS_OPTION_POSITIONS};
	ButtonsOptionPosition buttonsOptionPosition;

	bool isChangingSetting; // This bool means if player is trying to change any setting

	// This is used to check what options have been changed by you
	Options currentOptions;

private:
	// Used to write in xml
	std::ostringstream outputStringStream;

public:
	OptionsState();
	~OptionsState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "options_state"; }

	void onStartElement (const std::string &elem, MKeyValue &atts);

protected:
	void update( float deltaTime );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void loadGameState( );

private:
	void updateSetting( );
	void updateButtonsSettings( );

	void updateResolution( );
	void updateQuality( );
	void updateMusic( );
	void updateSFX( );
	void udpatePositionInButtons( );

	void applyChanges( );

	void changeQuality( );

	const std::string &getQualityString( Quality pQuality ) const;
	Quality getQuality( const std::string &pQuality ) const;

	void printfKeyKeyboard( float y, const char *msg, ... );
};

#endif // OPTIONS_STATE_H
