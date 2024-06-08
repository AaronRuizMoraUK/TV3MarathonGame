#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include "GameOptions.h"

/**
* Game application with windows gestion
*/

class Game
{

protected:
	float deltaTime;
	static float game_speed;

	// Variable to calculate FPS
	static float numFrames;

	static bool isGameInitiated;
	static bool exit_game;

public:
	// Frames per second
	static float fps;
	static float shader_time; // Suma de todos los delta time desde que empezó el juego para dar continuidad a los shaders

	// Config Options
	static Options configOptions;
	static Resolutions allowedResolutions;

public:
	Game();
	~Game();

	bool initiate();
	void run();
	void clean();

	static void exitGame( ) { exit_game = true; }

private:
	// Window class and handler
	WNDCLASSEX wc;
	HWND hWnd;
	bool isClassRegistered;

	bool createWindow();
	bool initDevice();
	bool initGlobals();

	friend LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};

#endif // GAME_H
