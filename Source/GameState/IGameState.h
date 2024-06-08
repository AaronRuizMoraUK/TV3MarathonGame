#ifndef IGAME_STATE_H
#define IGAME_STATE_H

#include "Util/RenderTexture/RenderTexture.h"
#include "Object/Object.h"
#include "Common/DinamicHelp.h"
#include "Common/Pause.h"

/**
* Game state
*/

class IGameState
{
public:
	static const int numStates = 30;
	static IGameState *gameStates[numStates];
	static IGameState *currentState;

private:
	// Render to texture
	static RenderTexture renderTexture;
	static Object        renderTextureQuad;

	// Dinamic Help
	static DinamicHelp   dinamicHelp;
	static bool          dinamic_help_enabled;

	// Pause
	static Pause         pause;
	static bool					 pause_enabled;

	static bool take_screenshot;

public:
	IGameState() {}
	virtual ~IGameState() {}

	void updateFrame( float deltaTime );
	void render( );

	virtual void onLoad( ) = 0;
	virtual void onUnload( ) = 0;
	virtual const char *getName( ) const = 0;

	void getFocus( );

	static IGameState * getState( const char *name );

	static void showLoadingScreen( );
	static void showDinamicHelp( DinamicHelp::DinamicHelpDesc help );
	static void showPause( const std::string &pause_on_exit_state );
	static void takeScreenshot( ) { take_screenshot = true; }
	
	static DinamicHelp::DinamicHelpDesc getDinamicHelpDesc( const char *desc_text ) { return dinamicHelp.getDinamicHelpDesc( desc_text ); }

	// Used in game.cpp
	static void load();
	static void destroy();

protected:
	virtual void loadGameState() {};

	virtual void beginRenderFrame();
	virtual void endRenderFrame();
	virtual void beginRenderTexture();
	virtual void endRenderTexture();

	virtual void update( float deltaTime ) = 0;

	virtual void preRender( ) = 0;
	virtual void renderWorld3D( ) = 0;
	virtual void renderWorld2D( ) = 0;
	virtual void renderPostProcessed2D( ) = 0;
	virtual void renderGameUI( ) = 0;
	virtual void postRender( ) = 0;

	virtual void renderFrame( );

	//bool isVisible(const Object &object);

private:
	void renderDinamicHelp( );
	void renderPause( );
};

#endif // IGAME_STATE_H
