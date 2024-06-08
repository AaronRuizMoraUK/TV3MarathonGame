#ifndef SECOND_LEVEL_UI_H
#define SECOND_LEVEL_UI_H

#include "Object/Object.h"
#include "Util/StringRender/UINumberRender.h"
#include "Util/Timeout/Timeout.h"
#include "Util/Fade/TextureFade.h"

/**
* This class make the User Interface for
* Second Level
*/

class SecondLevelUI
{
protected:
	// UI Objects
	Object sync_level_picture;
	Object lifes_picture;
	Object score_picture;
	Object bomba_picture;
	Object bomba_serotonina_picture;
	Object bomba_noradrenalina_picture;

	UINumberRender numbers;
	
	static const float showWelcomeMsgTime;
	static const float showWelcomeMsgFadeTime;
	float welcomeMessageAlpha;
	Timeout timeoutShowWelcomeMsg;
	TextureFade welcomeMessage;

public:
	SecondLevelUI( );
	~SecondLevelUI( );

	void load( );
	void reset( );

	void showWelcomeImage( );

	void update( float deltaTime );
	void render( );
};

#endif // SECOND_LEVEL_UI_H
