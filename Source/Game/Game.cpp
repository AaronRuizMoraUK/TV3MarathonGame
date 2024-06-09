#include <cassert>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include "Game.h"
#include "resource.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Camera/ICamera.h"
#include "Camera/Camera3D.h"
#include "Camera/CameraOrtho.h"
#include "GameState/IGameState.h"
#include "IO/IOStatus.h"
#include "Mesh/VertexTypes.h"
#include "Lua/LuaVM.h"
#include "SoundPlayer/SoundPlayer.h"

//#define DEBUG_NVPERFHUD;

// ---------------------------------------------------------------------

float Game::game_speed = 1.0f;
float Game::numFrames = 0.0f;
float Game::fps = 0.0f;
float Game::shader_time = 0.0f;

bool  Game::isGameInitiated = false;
bool  Game::exit_game = false;

Options     Game::configOptions;
Resolutions Game::allowedResolutions;

Game::Game()
: deltaTime( 0.0f )
, isClassRegistered(false)
{
	// Hide mouse cursor
	ShowCursor(FALSE);
}

Game::~Game() {
	printDebugString("Destroying Game...\n");

	// Unregister the window class
	if( isClassRegistered )
		UnregisterClass( "Game", wc.hInstance );

	// Show mouse cursor
	ShowCursor(TRUE);
}

bool Game::initiate() {
	// Read config options of game
	if( !configOptions.load( ) )
		return false;

	// Create window
	if( !createWindow() )
		return false;

	// Init Device
	if( !initDevice() )
		return false;

	// Init Game
	if( !initGlobals() )
		return false;

	isGameInitiated = true;
	return true;
}

void Game::run() {

	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );

	// Delta time variables
	unsigned long time1=0;
	unsigned long time2=0;
	timeBeginPeriod(1);
	time1 = timeGetTime(); // Get first time

	while( true )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
			// If message is QUIT
			if(msg.message==WM_QUIT)
				break;

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else {
			// FIX DELTA TIME WITH TOO MUCH TIME
			if( deltaTime > 1.0f/20.0f )
				deltaTime = 1.0f/20.0f;

			// Update IO Status
			ioStatus.update( deltaTime );

			// Update all scene
			IGameState::currentState->updateFrame( deltaTime );

			if( exit_game )
				break;

			// Draw the scene
			IGameState::currentState->render( );

			// Get time to calculate delta time
			time2  = timeGetTime();
			deltaTime=((float)(time2-time1))*0.001f;
			time1 = time2;

			// Change Game speed
			deltaTime *= game_speed;

			// Update global time varible for shaders
			shader_time += deltaTime;

			// Increment the number of renders for FPS
			numFrames++;
		}
	}
}

/**
* Releases and destroy all previously initialized objects
*/
void Game::clean() {
	printDebugString("Begin Game Clean...\n");

	// Call LUA to destroy everything
	Global::lua_vm.executeCmd( "destroy_everything( )" );
	// NO GLOBAL VARIABLES CAN CALL A SINGLENTON IN ITS DESTRUCTION

	if( CUSTOMVERTEX_POSITION_DECLARATION != NULL )
		CUSTOMVERTEX_POSITION_DECLARATION->Release(), CUSTOMVERTEX_POSITION_DECLARATION=NULL;

	if( CUSTOMVERTEX_TEXTURE_DECLARATION != NULL )
		CUSTOMVERTEX_TEXTURE_DECLARATION->Release(), CUSTOMVERTEX_TEXTURE_DECLARATION=NULL;

	if( CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION != NULL )
		CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION->Release(), CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION=NULL;

	if( CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION != NULL )
		CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION->Release(), CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION=NULL;

	if( CUSTOMVERTEX_COLOR_DECLARATION != NULL )
		CUSTOMVERTEX_COLOR_DECLARATION->Release(), CUSTOMVERTEX_COLOR_DECLARATION=NULL;

	if( CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION != NULL )
		CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION->Release(), CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION=NULL;

	if( CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION != NULL )
		CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION->Release(), CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION=NULL;

	if( Global::font != NULL )
		Global::font->Release(), Global::font=NULL;

	if( Global::device != NULL )
		Global::device->Release(), Global::device=NULL;

	if( Global::deviceCreator != NULL )
		Global::deviceCreator->Release(), Global::deviceCreator=NULL;
}

/**
* The window's message handler
*
* @param hWnd Handler of the windows
* @param msg Message to be processed
* @param wParam Information about the msg
* @param lParam Information about the msg
* @return LRESULT Return value of the processing of the message
*/
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
#ifndef NDEBUG
	case WM_CHAR:
		{
			char chCharCode = (char) wParam;

			if( isPushed( 'L' ) ) {
				if( ioStatus.isInputDisabled( ) )
					ioStatus.enableInput( );
				else
					ioStatus.disableInput( );
			}
			if( !ioStatus.isInputDisabled( ) ) {

				// ESCAPE was pressed, so the application close
				//if(chCharCode==VK_ESCAPE) {
				//	PostMessage(hWnd, WM_CLOSE, NULL, NULL); // Post a WM_CLOSE message
				//}

				// CHANGE GAME SPEED
				if( isPushed( '3' ) )
					Game::game_speed = 1.0f;
				else if( isPushed( '0' ) )
					Game::game_speed = 0.0f;
				else if( isPushed( '1' ) )
					Game::game_speed = 1.0f/20.0f;
				else if(isPushed( '2' ) )
					Game::game_speed = 1.0f/5.0f;
				else if( isPushed( '4' ) )
					Game::game_speed = 5.0f;

				// RELOADS
				if( isPushed( 'Z' ) )
					Global::lua_vm.executeCmd( "reload_textures( )" );	
				if( isPushed( 'X' ) )
					Global::lua_vm.executeCmd( "reload_shaders( )" );	
				if( isPushed( 'C' ) )
					Global::lua_vm.executeCmd( "reload_planetmaps( )" );	
				if( isPushed( 'V' ) )
					Global::lua_vm.executeCmd( "reload_roa_resources( )" );	
				if( isPushed( 'B' ) )
					Global::lua_vm.executeCmd( "reload_animations( )" );

				// ENABLE/DISABLE RENDER TO TEXTURE
				if( isPushed( 'H' ) )
					Game::configOptions.renderTextureEnabled = !Game::configOptions.renderTextureEnabled;

				// TAKE A PICTURE
				if( isPushed( 'J' ) ) {
					if( Game::configOptions.renderTextureEnabled )
						IGameState::takeScreenshot( );
				}

				// SHOW DINAMIC HELP
				if( isPushed( 'K' ) )
					IGameState::showDinamicHelp( DinamicHelp::PLANET1_FIRST_MESSAGE );

				//char msg[ 128 ];
				//sprintf_s(msg, sizeof(msg), "KEY value as INT: %d\n", chCharCode);
				//printDebugString(msg);
			}
		}
		break;
#endif

	case WM_TIMER:
		{
			// Number of frames per second
			Game::fps=Game::numFrames;
			Game::numFrames=0;
		}
		break;

	case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}
		break;
	}

	// Process any message that we did not take care of
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

bool Game::createWindow( )
{
	static const char className[] = "Game";

	// Register the window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		className, NULL };
	wc = wcex;
	wc.hIcon = LoadIcon(wc.hInstance, (LPCTSTR)IDI_ICON);
	//wc.hIconSm = LoadIcon(wc.hInstance, (LPCTSTR)IDI_ICON_SMALL);
	if(!RegisterClassEx( &wc ))
		return false;

	// At this point the window class has been registered, so at the end must be unregistered
	isClassRegistered=true;

	// -----------------------------------------------------------------------------------------

	// Create the deviceCreator object
	if( NULL == ( Global::deviceCreator = createDeviceCreator() ) )
		return false;

	// Check if D3DFMT_X8R8G8B8 BackBufferFormat is supported and get allowed resolutions
	Resolution init_resolution;
	allowedResolutions.insert( init_resolution ); // Sure 800x600:60
	init_resolution.set( 1024, 768, 60 );
	allowedResolutions.insert( init_resolution ); // Sure 1024x768:60

	UINT numberModeCount = Global::deviceCreator->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	char msg[ 128 ];
	for(UINT i=0; i<numberModeCount; ++i) {
		D3DDISPLAYMODE displayMode;

		if( FAILED ( Global::deviceCreator->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &displayMode) ) ) {
			//sprintf_s(msg, sizeof(msg), "Not supported: %d\n", i);
			//printDebugString(msg);
			continue;
		}
		else {
			sprintf_s(msg, sizeof(msg), "i: %d Width: %d Height: %d RefreshRate: %d\n", i,
				displayMode.Width, displayMode.Height, displayMode.RefreshRate);
			printDebugString(msg);

			//if( displayMode.Width >=800 && displayMode.Width <=1920 &&
			//	(displayMode.RefreshRate == 60 || displayMode.RefreshRate == 70 ||
			//	displayMode.RefreshRate == 75 || displayMode.RefreshRate == 85) )
			{
				Resolution newResolution;
				newResolution.set(displayMode.Width, displayMode.Height, displayMode.RefreshRate);
				allowedResolutions.insert( newResolution );
			}
		}
	}

	if( configOptions.fullScreen ) {
		// If resolution in config.xml is not in allowedResolutions, put 800x600:60
		Resolutions::const_iterator res_it = allowedResolutions.find( configOptions.resolution );
		if( res_it == allowedResolutions.end() )
			configOptions.resolution.set(800, 600, 60);
	}

	// -----------------------------------------------------------------------------------------

	// Create the application's window
	if( !(hWnd = CreateWindow(className, className,
		(configOptions.fullScreen) ? WS_POPUP : WS_OVERLAPPEDWINDOW, 0, 0, configOptions.resolution.width, configOptions.resolution.height,
		GetDesktopWindow(), NULL, wc.hInstance, NULL)) )
		return false;

	// Show the window
	ShowWindow( hWnd, SW_SHOWDEFAULT );
	UpdateWindow( hWnd );

	return true;
}

bool Game::initDevice()
{
	HRESULT hr;

	// Set up the structure used to create the Device with z-buffer
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // D3DSWAPEFFECT_DISCARD D3DSWAPEFFECT_COPY
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; // D3DFMT_D24S8 D3DFMT_D16
	// Antialliasing. OFF=D3DMULTISAMPLE_NONE 4x=D3DMULTISAMPLE_4_SAMPLES
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	// VSync. OFF=D3DPRESENT_INTERVAL_IMMEDIATE ON=D3DPRESENT_INTERVAL_ONE
	d3dpp.PresentationInterval = configOptions.vSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	// Full Screen
	d3dpp.Windowed = !configOptions.fullScreen;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; // D3DFMT_A8R8G8B8 D3DFMT_UNKNOWN D3DFMT_X8R8G8B8
	if(configOptions.fullScreen) {
		d3dpp.BackBufferWidth = configOptions.resolution.width;
		d3dpp.BackBufferHeight = configOptions.resolution.height;
		if( configOptions.resolution.rate != -1 )
			d3dpp.FullScreen_RefreshRateInHz = configOptions.resolution.rate;
	}

#ifndef DEBUG_NVPERFHUD
	// Create the device with hardware vertex processing
	if( FAILED( Global::deviceCreator->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &Global::device ) ) )
	{
		// If failed, try to create the device with software vertex processing
		if( FAILED( Global::deviceCreator->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &Global::device ) ) ) {
				return false;
		}
		else {
			// If the graphic card does not support D3DCREATE_HARDWARE_VERTEXPROCESSING is
			// because does not have Hardware T&L, that means the graphic card is very old, so
			// set the quality to low
			configOptions.quality = "baix";
			configOptions.renderTextureEnabled = false;
		}
	}
	assert( Global::device || !"Device not created" );
#else
	// Set default settings
	UINT AdapterToUse=D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL;

#if SHIPPING_VERSION
	// When building a shipping version, disable NVPerfHUD (opt-out)
#else
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT Adapter=0;Adapter<Global::deviceCreator->GetAdapterCount();Adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res;
		Res = Global::deviceCreator->GetAdapterIdentifier(Adapter,0,&Identifier);
		if (strstr(Identifier.Description,"NVPerfHUD") != 0)
		{
			AdapterToUse=Adapter;
			DeviceType=D3DDEVTYPE_REF;
			break;
		}
	}
#endif

	if (FAILED(Global::deviceCreator->CreateDevice( AdapterToUse, DeviceType, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &Global::device) ) )
	{
		return false;
	}
#endif

	// INIT RENDER STATE
	initRenderState( );

	// FOR PRINTING
	D3DXFONT_DESC fnt;
	memset(&fnt, 0, sizeof(D3DXFONT_DESC));
	fnt.Width = 7;
	fnt.Height = 12;
	fnt.Italic = true;
	strcpy_s (fnt.FaceName, sizeof(fnt.FaceName), "arial");
	D3DXCreateFontIndirect (Global::device, &fnt, &Global::font);

	// Create Vertex declarations
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_POSITION_ELEMENT, &CUSTOMVERTEX_POSITION_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_TEXTURE_ELEMENT, &CUSTOMVERTEX_TEXTURE_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_NORMAL_TEXTURE_ELEMENT, &CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_NORMAL_TAN_TEXTURE_ELEMENT, &CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_COLOR_ELEMENT, &CUSTOMVERTEX_COLOR_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_TEXTURE_LIGHTMAP_ELEMENT, &CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION);
	assert(hr==D3D_OK);
	hr = Global::device->CreateVertexDeclaration( CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_ELEMENT, &CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION);
	assert(hr==D3D_OK);

	return true;
}

bool Game::initGlobals() {
	// Set a timer to go off 1 times a second, so calculate in MsgProc function the FPS
	SetTimer( hWnd, 0, 1000, NULL );

	// Random seed
	srand( (unsigned)time( 0 ) );

	// CAL3D INITIATE
	CalLoader::setLoadingMode(LOADER_ROTATE_X_AXIS); // Rotate -90 degree in X axis

	// Initiate data locator
	Global::data_locator.initiate( );

	// Initiate cameras
	getCamera3D( ).initiate( );
	getCameraOrtho( ).initiate( );

	// Show loading screen
	IGameState::showLoadingScreen( );
	activeCamera3D( ); // Dejar la camara 3D activada

	// Initiate IO Status
	ioStatus.initiate( hWnd );

	// ------------------------------------------------------------------

	// Load LUA
	Global::lua_vm.load();

	// Call LUA to load Everything Common and init game
	bool ok = Global::lua_vm.execute( "init_game.lua" );

	return ok;
}
