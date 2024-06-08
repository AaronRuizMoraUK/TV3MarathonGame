#include "player.h"
#include "Global/Rendering.h"
#include "PlayerControllers/IPlayerController.h"
#include "PlayerControllers/PlaneController/PlaneController.h"
#include "PlayerControllers/SpherePlayerController/SpherePlayerController.h"

bool Player::player_controller_enabled = true;

Player::Player()
: player_controller(NULL)
{
}

Player::Player(const Player &source)
: CalAnimatedObject(source)
, player_controller(NULL)
{
	copy(source);
}

// Destructor
Player::~Player() {
	printDebugString("Destroying Player...\n");
	destroy();
}

// Override Operator '='
void Player::operator=(const Player &source) {
	CalAnimatedObject::operator=(source);
	copy(source);
}

void Player::destroy() {
	// Destroy Player controller
	if (player_controller)
		delete player_controller, player_controller=NULL;
}

void Player::copy(const Player &source) {
	destroy( );

	if( source.player_controller != NULL ) {
		const char *controller_type = source.player_controller->getControllerType();

		if( strcmp( controller_type, "PlaneController" ) == 0 )
			player_controller = new PlaneController( *((PlaneController *) source.player_controller) );
		else if( strcmp( controller_type, "SpherePlayerController" ) == 0 )
			player_controller = new SpherePlayerController( *((SpherePlayerController *) source.player_controller) );
		else {
			assert( !"Not type of Player controller allowed in Player" );
			return;
		}
	}
}

void Player::updateController(float deltaTime) {
	if (player_controller && player_controller_enabled)
		player_controller->update(*this, deltaTime);
}

void Player::setPlayerController(IPlayerController *aplayer_controller) {
	if (player_controller)
		delete player_controller, player_controller=NULL;
	player_controller=aplayer_controller;
}

void Player::processSignal( TSignal tag, const void *data, size_t num_bytes ) {
	if(player_controller)
		player_controller->processSignal( tag, data, num_bytes );
}

void Player::addSignalListener( SignalListener *listener ) {
	if( listener != NULL )
		signal_listeners.push_back( listener );
}

void Player::removeSignalListener( SignalListener *listener ) {
	if( listener != NULL )
		signal_listeners.remove( listener );
}

void Player::sendSignal( TSignal tag, void *data, size_t num_bytes ) {
	SignalListeners::iterator it = signal_listeners.begin( );
	while( it != signal_listeners.end( ) ) {
		(*it)->processSignal( tag, data, num_bytes );
		++it;
	}
}

void Player::collisionWith( const Object &object, CollisionType collision_type, const D3DXVECTOR3 &normal ) {
	if( player_controller )
		player_controller->collision( *this, object, collision_type, normal );
}
