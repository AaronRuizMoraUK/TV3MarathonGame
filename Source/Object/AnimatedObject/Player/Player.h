#ifndef PLAYER_H
#define PLAYER_H

#include "Object/AnimatedObject/CalAnimatedObject.h"

class IPlayerController;

/**
* This is player. It is an CalAnimatedObject with a controller
*/

class Player : public CalAnimatedObject
{
public:
	static bool player_controller_enabled;

protected:
	IPlayerController *player_controller;

	// Lista de listener que quieren escuchar señales que manda el player a traves de su player_controller
	typedef std::list<SignalListener *> SignalListeners;
	SignalListeners signal_listeners;

public:
	Player();
	Player(const Player &source);
	~Player();

	void operator=(const Player &source);

	void updateController(float deltaTime);

	IPlayerController * getPlayerController() const {return player_controller;}
	void setPlayerController(IPlayerController *aplayer_controller);

	// Para recibir señales de los demas. Uso -> Global::player->processSignal( ).
	void processSignal( TSignal tag, const void *data, size_t num_bytes );

	// Para recibir señales del player
	void addSignalListener( SignalListener *listener );

	// Para dejar de recibir señales del player (ANTES DE MORIR DESREGISTRARSE)
	void removeSignalListener( SignalListener *listener );

	// Usado desde los player_controller para mandar señales a su signal_listeners
	void sendSignal( TSignal tag ) { sendSignal( tag, 0, 0 ); }
	void sendSignal( TSignal tag, void *data, size_t num_bytes );

	void collisionWith( const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal );

private:
	void destroy();
	void copy(const Player &source);
};

#endif //PLAYER_H
