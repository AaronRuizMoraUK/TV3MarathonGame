#ifndef FIRST_LEVEL_UI_H
#define FIRST_LEVEL_UI_H

#include "Object/Object.h"
#include "Util/StringRender/UINumberRender.h"
#include "Util/Timeout/Timeout.h"
#include "Util/Fade/TextureFade.h"

/**
* This class make the User Interface for
* First Level
*/

class FirstLevelUI
{
protected:
	Object dopamina_bar;
	Object dopamina_bar_pivot;
	static const D3DXVECTOR3 start_bar_pos;
	static const float       dopamina_bar_width;
	D3DXVECTOR3 dopamina_bar_pivot_pos;

	Object sync_level_picture;
	Object lifes_picture;
	Object score_picture;

	UINumberRender numbers;
	
	static const float showWelcomeMsgTime;
	static const float showWelcomeMsgFadeTime;
	float welcomeMessageAlpha;
	Timeout timeoutShowWelcomeMsg;
	TextureFade welcomeMessage;

private:
	float test_perc;

public:
	FirstLevelUI( );
	~FirstLevelUI( );

	void load( );
	void reset( );

	void showWelcomeImage( );

	void update( float deltaTime );
	void render( );

private:
	float getXPivot( );
};

#endif // FIRST_LEVEL_UI_H
