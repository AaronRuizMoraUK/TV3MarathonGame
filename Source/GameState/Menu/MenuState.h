#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "GameState/IGameState.h"
#include "Expat/XMLParser.h"
#include "Object/AnimatedObject/AnimatedObject.h"

/**
* Menu game state
*/

class MenuState : public IGameState, public CXMLParser
{
public:
	// Objects in menu state
	AnimatedObject	menuPlanet;
	Object			background;
	Object			title;

	// Actions in menu state
	Object			startAction;
	Object			optionsAction;
	Object			creditsAction;
	Object			scoresAction;
	Object			exitAction;

	bool doOnLoad;

protected:
	// Menu State Actions
	enum MenuStateAction {
		START = 0
		, OPTIONS
		, CREDITS
		, SCORES
		, EXIT
		, NUM_MENU_STATE_ACTIONS
	};
	MenuStateAction menuStateAction;

	enum MenuStateEnum {
		FADING_IN = 0
		, CHOOSING_ACTION
		, FADING_OUT
	};
	MenuStateEnum menuState;

private:
	// Use just for reading from xml
	bool inPlanet;
	bool inStartAction;
	bool inOptionsAction;
	bool inCreditsAction;
	bool inScoresAction;
	bool inExitAction;
	bool inTitle;

public:
	MenuState();
	~MenuState();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "menu_state"; }

protected:
	void update( float deltaTime );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );
	
	void loadGameState();

private:
	// Use just for reading from xml
	void createPlanet(const std::string &elem, MKeyValue &atts);
	void createAction(Object &action, const std::string &elem, MKeyValue &atts);
	void createTitle(const std::string &elem, MKeyValue &atts);

	void updateMenuPlanet( float deltaTime );
};

#endif // MENU_STATE_H
