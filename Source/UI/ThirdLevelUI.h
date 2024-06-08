#ifndef THIRD_LEVEL_UI_H
#define THIRD_LEVEL_UI_H

#include "Object/Object.h"
#include "Util/StringRender/UINumberRender.h"
#include "Util/Timeout/Timeout.h"
#include "Util/Fade/TextureFade.h"
#include "Util/StringRender/AlphabetRender.h"

/**
* This class make the User Interface for
* Third Level
*/

class ThirdLevelUI
{
protected:
	// UI Objects
	Object sync_level_picture;
	Object lifes_picture;
	Object score_picture;

	UINumberRender numbers;
	CatalanAlphabetRender alphabet;

	Object valve_bar;
	Object valve_no_ok;
	Object valve_ok;
	
	static const float showWelcomeMsgTime;
	static const float showWelcomeMsgFadeTime;
	float welcomeMessageAlpha;
	Timeout timeoutShowWelcomeMsg;
	TextureFade welcomeMessage;

public:
	ThirdLevelUI( );
	~ThirdLevelUI( );

	void load( );
	void reset( );

	void showWelcomeImage( );

	void update( float deltaTime );
	void render( );
};

#endif // THIRD_LEVEL_UI_H
