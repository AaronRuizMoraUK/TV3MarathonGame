#include "EndSecondLevelState.h"
#include "Logic/SecondLevelLogic.h"
#include "Global/Variables.h"
#include "Global/Rendering.h"
#include "Source/Game.h"
#include "Camera/Camera3D.h"
#include "Bass/SoundPlayer.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Factories/MeshFactory.h"
#include "Factories/QuadFactory.h"

EndSecondLevelState::EndSecondLevelState()
: next_instruction( InstructionsState::NONE )
, cilindroNoradrenalina(NULL),
	cilindroSerotonina(NULL)
{
}

EndSecondLevelState::~EndSecondLevelState() {
	if (cilindroNoradrenalina)
		delete cilindroNoradrenalina, cilindroNoradrenalina = NULL;

	if (cilindroSerotonina)
		delete cilindroSerotonina, cilindroSerotonina = NULL;

	printDebugString("Destroying EndSecondLevelState...\n");
}

void EndSecondLevelState::update( float deltaTime ) {
	// Update Camera
	getCamera3D( ).update( deltaTime );

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	// Update AIs
	getSecondLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getSecondLevelLogic( ).power_ups.updateAnimation( deltaTime );

	// Spatial and Collisions
	Global::spatial.update( deltaTime );
	Global::collisions.update( deltaTime );

	// Update Particles
	PSManager::get( )->update( deltaTime );

	if( !Global::fade.isFinished( ) )
		Global::fade.update( deltaTime );
	else {
		InstructionsState *instructions_state = (InstructionsState *) getState("instructions_state");
		instructions_state->instruction = next_instruction;
		instructions_state->getFocus();
		return;
	}
}

void EndSecondLevelState::preRender( ) {
}

void EndSecondLevelState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	Global::player->render();
	BrainManager::get( )->getCurrentBrain( ).render();
	Global::sky.render();

	// Render
	if (getSecondLevelLogic().bomba)
	{
		getSecondLevelLogic().bomba->render();
	}

	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void EndSecondLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );
	
	getSecondLevelLogic( ).power_ups.render( );

	// Render Particles
	PSManager::get( )->render( );
	cilindroNoradrenalina->render();
	cilindroSerotonina->render();

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void EndSecondLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	renderStateDisableAlphaBlending( );
}

void EndSecondLevelState::renderGameUI( ) {
}

void EndSecondLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "End Second Level Status");
#endif

	renderStateEnableAlphaBlending( );
	Global::fade.renderFadeOut( 0.5f );
	renderStateDisableAlphaBlending( );
}

void EndSecondLevelState::onLoad( ) {
	assert( next_instruction != InstructionsState::NONE );
	SoundPlayer::get( )->stopAll( true );
}

void EndSecondLevelState::onUnload( ) {
	next_instruction = InstructionsState::NONE;
	getSecondLevelLogic( ).destroyLevel( );
}

void EndSecondLevelState::loadGameState( )
{
	// Vignetting
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		vignetting.SETMATERIAL("vignetting_pan.dds", "shader.fx", "dummy_tech");
	else
		vignetting.SETMATERIAL("vignetting.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	vignetting.setMesh(mesh);

	cilindroSerotonina = CrearCilindro("recipiente.mesh", "cilindro_blue.dds",1.4666f, -1.345f);
	cilindroNoradrenalina = CrearCilindro("recipiente.mesh", "cilindro_red.dds", 4.91f, -1.26f);
}

Object * EndSecondLevelState::CrearCilindro(const std::string &mesh_name, const std::string &texture, float alpha, float delta)
{
	Object *obj = new Object();
	const IMesh *mesh = MeshFactory::get()->createMesh(mesh_name);

	assert(mesh);
	obj->setMesh(mesh);
	obj->SETMATERIAL(texture, "shader.fx", "basic_tech");

	float rot = 0.0f;

	obj->loc.setAlpha( alpha );
	obj->loc.setDelta( delta );

	D3DXMATRIX transform;
	BrainManager::get( )->getCurrentBrain( ).getTransformOnPlanet( transform, rot, alpha, delta );
	obj->loc.setTransform( transform );

	return obj;
}
