//- ACORDARSE DE REPASAR LOS isPushed
//- QUITAR LOS PRINTF ABUSIVOS DE LOS postRender()
//- ACORDARSE DE QUITAR TODOS LOS Global::DrawXXX -> Cuidado en los preRender()
//- ACORDARSE DE PONER sinf, cosf, sqrtf, fabs, atan2f, powf, ETC.
//- ACORDARSE DE QUITAR LOS RENDERS DE LOS VOLUMENES LÓGICOS (SI LOS HAY)

#include <cassert>
#include "Game.h"

/**
* The application's entry point
*
* @param HINSTANCE HINSTANCE LPSTR INT Application's entry parameters
* @return INT Return value of the application
*/
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	// Create game
	Game game;

	// Initiate game
	if( game.initiate() ) {
		// Run game
		game.run();
	}
	else {
		assert( !"Not initiated" );
		return 0;
	}

	// Clean game
	game.clean();

	return 0;
}
