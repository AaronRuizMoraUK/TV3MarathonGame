#ifndef FOURTH_LEVEL_UI_H
#define FOURTH_LEVEL_UI_H

#include "Object/Object.h"
#include "Util/StringRender/UINumberRender.h"
#include "Util/Timeout/Timeout.h"
#include "Util/Fade/TextureFade.h"

/**
* This class make the User Interface for
* Fourth Level
*/

class FourthLevelUI
{
protected:
	// UI Objects
	Object bar;
	Object dopamina_bar_pivot;
	Object noradrenalina_bar_pivot;
	static const D3DXVECTOR3 start_bar_pos;
	static const float       bar_width;
	D3DXVECTOR3 dopamina_bar_pivot_pos;
	D3DXVECTOR3 noradrenalina_bar_pivot_pos;

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
	FourthLevelUI( );
	~FourthLevelUI( );

	void load( );
	void reset( );

	void showWelcomeImage( );

	void update( float deltaTime );
	void render( );

private:
	float getDopaminaXPivot( );
	float getNoradrenalinaXPivot( );
};

#endif // FOURTH_LEVEL_UI_H
