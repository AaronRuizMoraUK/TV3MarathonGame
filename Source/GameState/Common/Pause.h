#ifndef PAUSE_H
#define PAUSE_H

#include "Object/Object.h"
#include "Util/StringRender/AlphabetRender.h"

class Pause {
public:
	enum TChoice {
		RESUME = 0
		, EXIT
	};

protected:
	Object arrow;
	CatalanAlphabetColorRender alphabet;

	static const D3DXVECTOR3 onContinuePosition;
	static       D3DXVECTOR3 onBackToMenuPosition;

	TChoice choice;

	bool fading_in_pause;
	bool fading_out_pause;

	std::string pause_on_exit_state;

	// Flowers pictures
	Object flowerUp;
	Object flowerDown;

	int samples_volume;

public:
	Pause( );
	~Pause( );

	void load( );
	void initiate( const std::string &apause_on_exit_state );
	TChoice getChoice( ) const { return choice; }
	void goToExitState( );

	bool update( float deltaTime );
	void render( );
};

#endif //PAUSE_H
