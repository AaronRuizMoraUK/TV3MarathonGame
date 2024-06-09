#include "ParticleState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "Global/Geometry.h"
#include "IO/IOStatus.h"
#include "Game/Game.h"
#include "Camera/Camera3D.h"
#include "Particles/PSManager.h"
#include "SoundPlayer/SoundPlayer.h"
#include "Util/Tracker/Tracker.h"

//#include "Factories/MeshFactory.h"

void ParticleState::updateCamera( float deltaTime ) {
	static const float speed = 5.0f;
	static const float rotationSpeed = 1.0f;
	Camera3D &camera = getCamera3D( );

	// Get the last position and target of the camera
	const D3DXVECTOR3 &prevPosition = camera.getPosition();
	const D3DXVECTOR3 &prevTarget = camera.getTarget();

	// Get the last camera local front vector
	const D3DXVECTOR3 &prevLocalFrontVector = camera.getLocalFrontVector();
	D3DXVECTOR3 newLocalFrontVector = prevLocalFrontVector;

	// Delta vector
	D3DXVECTOR3 delta(0, 0, 0);

	// Check Controllers
	const D3DXVECTOR2 &left_thumb_dir = ioStatus.xboxGetThumbLeftDirection( );
	const D3DXVECTOR2 &right_thumb_dir = ioStatus.xboxGetThumbRightDirection( );

	delta += left_thumb_dir.y*camera.getLocalFrontVector() - left_thumb_dir.x*camera.getLocalLeftVector();

	if (ioStatus.xboxIsPressed( "LEFT_TRIGGER" ))
		delta += D3DXVECTOR3(0,-1,0);
	if (ioStatus.xboxIsPressed( "RIGHT_TRIGGER" ))
		delta += D3DXVECTOR3(0,1,0);

	{
		// Rotation Matrix
		D3DXMATRIX matRotation;
		// Angle in radians of the rotation
		float radians = - right_thumb_dir.x * rotationSpeed * deltaTime;
		// Get the matrix rotation and applies to the previous local front vector
		D3DXMatrixRotationY(&matRotation, radians);
		D3DXVec3TransformNormal(&newLocalFrontVector, &newLocalFrontVector, &matRotation);
	}
	{
		float prevYaw, prevPitch;
		vectorToAngles(newLocalFrontVector, prevYaw, prevPitch);

		if(prevPitch > -D3DX_PI/2+0.01) {
			float radians = - right_thumb_dir.y * rotationSpeed * deltaTime;
			float newPitch = prevPitch+radians;
			anglesToVector(prevYaw, newPitch, newLocalFrontVector);
		}
	}

	// Update delta vector
	delta *= speed*deltaTime;

	// Set the up vector of the camera
	camera.setUpVector(D3DXVECTOR3(0,1,0));

	// Update where camera is looking at
	// NOTE: prevTarget is equal to prevPosition+prevLocalFrontVector
	camera.lookAt(prevPosition+delta, prevPosition+newLocalFrontVector+delta);
}

ParticleState::~ParticleState() {
	printDebugString("Destroying ParticleState...\n");
}

void ParticleState::update( float deltaTime ) {
	// Go to menu
	if( isPushed( 'Q' ) ) {
		getState( "menu_state" )->getFocus( );
		return;
	}

	// Update Camera
	updateCamera( deltaTime );

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	static bool lanzar = false;
	static bool prev_lanzar = false;
	static bool borrar = false;
	static bool prev_borrar = false;
	static bool recargar = false;
	static bool prev_recargar = false;
	static bool siguiente = false;
	static bool prev_siguiente = false;
	static bool anterior = false;
	static bool prev_anterior = false;

	prev_lanzar = lanzar;
	lanzar = isPushed( 'E' );
	prev_borrar = borrar;
	borrar = isPushed( 'D' );
	prev_recargar = recargar;
	recargar = isPushed( 'R' );
	prev_siguiente = siguiente;
	siguiente = isPushed( 'W' );
	prev_anterior = anterior;
	anterior = isPushed( 'S' );

	// Update loc
	//D3DXMATRIX rot;
	//rotateOnAxis( rot, loc.getTransform( ), D3DXVECTOR3(0,0,1), deltaTime );
	//loc.setTransform( rot );
	//loc.setPosition( loc.getPosition( ) + D3DXVECTOR3(0,0,deltaTime) );

	// Update XBOX IO
	if( ioStatus.xboxBecomesPressed( "A" ) || (lanzar && !prev_lanzar) ) {
		const std::string &core_name = PSManager::get( )->getNameCore( current_core_particle );
		loc = TLocation( );
		PSManager::get( )->addParticleGroup( core_name, TTracker( &loc ) );
	}
	else if( ioStatus.xboxBecomesPressed( "B" ) || (borrar && !prev_borrar) )
		PSManager::get( )->killAll( );
	else if( ioStatus.xboxBecomesPressed( "LEFT_SHOULDER" ) || (recargar && !prev_recargar) ) {
		PSManager::get( )->reload( );
		if( current_core_particle > PSManager::get( )->getNumCores( ) )
			current_core_particle = PSManager::get( )->getNumCores( ) - 1;
	}
	else if( ioStatus.xboxBecomesPressed( "DPAD_UP" ) || (siguiente && !prev_siguiente) )
		current_core_particle = (current_core_particle + 1) % PSManager::get( )->getNumCores( );
	else if( ioStatus.xboxBecomesPressed( "DPAD_DOWN" ) || (anterior && !prev_anterior) )
		current_core_particle = (current_core_particle + PSManager::get( )->getNumCores( ) - 1) % PSManager::get( )->getNumCores( );

	// Update Particles
	PSManager::get( )->update( deltaTime );
}

void ParticleState::preRender( ) {
	// --- Debug draws --- //
	activeMatsOnDeviceForDrawsUP( );
	drawGridXZ( );
	drawAxis( );
}

void ParticleState::renderWorld3D( ) {
	//renderStateEnableZTest( );
	//renderStateEnableZWriting( );

    //sphere.render( );

	//renderStateDisableZWriting( );
	//renderStateDisableZTest( );
}

void ParticleState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void ParticleState::renderPostProcessed2D( ) {
}

void ParticleState::renderGameUI( ) {
}

void ParticleState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0,  py, 0xffff0000, "Particle Status");
	py += 10;

	const std::string &core_name = PSManager::get( )->getNameCore( current_core_particle );
	py += printf(0, py, 0xffff0000, "Core %d: \"%s\"", current_core_particle, core_name.c_str( ) );
	py += 10;

	py += PSManager::get( )->printDebugInfo( py );
#endif
}

void ParticleState::onLoad( ) {
	// Stop Music
	SoundPlayer::get( )->stopAll( );

	// Place camera
	getCamera3D( ).setCameraController( NULL );
	getCamera3D( ).setUpVector( D3DXVECTOR3( 0,1,0 ) );
	getCamera3D( ).lookAt( D3DXVECTOR3(0,5,10), D3DXVECTOR3(0,0,0) );

	// Clean Particles
	PSManager::get( )->killAll( );

	current_core_particle = 0;

    //const IMesh *sphere_mesh = MeshFactory::get( )->createMesh( "Plane.mesh" );
    //assert( sphere_mesh );
    //sphere.setMesh( sphere_mesh );
    //sphere.SETMATERIAL( "rock.dds", "shader.fx", "normal_tech" );
}

void ParticleState::onUnload( ) {
	// Clean Particles
	PSManager::get( )->killAll( );
}
