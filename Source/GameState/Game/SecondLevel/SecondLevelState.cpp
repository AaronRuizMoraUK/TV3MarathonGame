#include "SecondLevelState.h"
#include "EndSecondLevelState.h"
#include "GameState/Game/ScoreRecountState.h"
#include "Logic/SecondLevelLogic.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Camera/Camera3D.h"
#include "Game/Game.h"
#include "Particles/PSManager.h"
#include "Managers/BrainManager.h"
#include "Object/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Factories/TextureFactory.h"
#include "Factories/MeshFactory.h"
#include "Mesh/IMesh.h"
#include "Factories/QuadFactory.h"

const float SecondLevelState::max_hurt_time = 1.0f;

SecondLevelState::SecondLevelState()
: hurt_time( 0.0f )
, cilindroNoradrenalina(NULL)
, cilindroSerotonina(NULL)
{
}

SecondLevelState::~SecondLevelState() {
	if (cilindroNoradrenalina)
		delete cilindroNoradrenalina, cilindroNoradrenalina = NULL;

	if (cilindroSerotonina)
		delete cilindroSerotonina, cilindroSerotonina = NULL;

	printDebugString("Destroying SecondLevelState...\n");
}

void SecondLevelState::update( float deltaTime ) {
	// GANAR
	if( getSecondLevelLogic( ).isLevelWon( )
#ifndef NDEBUG
		|| isPushed( 'G' )
#endif
		) {
		EndSecondLevelState *end_second_level_state = (EndSecondLevelState *)getState( "end_second_level_state" );
		end_second_level_state->next_instruction = InstructionsState::SCORE_RECOUNT;

		// Prepare that after Score Recount, we go to Init Third Level
		ScoreRecountState *score_recount_state = (ScoreRecountState *)getState( "score_recount_state" );
		score_recount_state->next_instruction = InstructionsState::INIT_THIRD_LEVEL;

		// Go to End Second Level State
		end_second_level_state->getFocus( );
		return;
	}
	// PERDER
	else if( getSecondLevelLogic( ).isLevelLost( ) 
#ifndef NDEBUG
		|| isPushed( 'P' ) 
#endif
		) {
		// Bajar una vida
		Logic::lifeDown( );

		// Hemos acabado el juego
		EndSecondLevelState *end_second_level_state = (EndSecondLevelState *)getState( "end_second_level_state" );
		if( Logic::isGameOver( ) )
			end_second_level_state->next_instruction = InstructionsState::GAME_OVER;
		else
			end_second_level_state->next_instruction = InstructionsState::LOSE_SECOND_LEVEL;
		end_second_level_state->getFocus( );
		return;
	}
	// PAUSE
	else if( ioStatus.start.becomesPressed() ) {
		showPause( "end_second_level_state" );
		return;
	}

	// -------------------------------------------

	// Update camera
	getCamera3D( ).update(deltaTime);

	// Update frustums
	Global::frustum.update();
	Global::back_side_frustum.update();

	// Update Controllers
	Global::player->updateController(deltaTime);

	// Update AIs
	if (getSecondLevelLogic().bomba)
	{
		getSecondLevelLogic().bomba->updateAI(deltaTime);
	}
	getSecondLevelLogic( ).power_ups.updateAI( deltaTime );

	// Update Animations
	Global::player->updateAnimation(deltaTime);
	Global::sky.updateAnimation(deltaTime);
	getSecondLevelLogic( ).power_ups.updateAnimation( deltaTime );

	// Spatial and Collisions
	Global::spatial.update( deltaTime );
	Global::collisions.update( deltaTime );

	// Update Logic and triggers
	getSecondLevelLogic( ).update( deltaTime );
	Global::triggers->update( deltaTime );

	// Update Particles
	PSManager::get( )->update( deltaTime );

	// Update UI
	second_level_ui.update( deltaTime );

	// Update Hurt
	if( hurt_time > 0.0f )
		hurt_time -= deltaTime;
}

void SecondLevelState::preRender( ) {
}

void SecondLevelState::renderWorld3D( ) {
	renderStateEnableZTest( );
	renderStateEnableZWriting( );

	// Game Renders
	Global::player->render();
 	BrainManager::get( )->getCurrentBrain( ).render();
	Global::sky.render();

	if (getSecondLevelLogic().bomba)
	{
		getSecondLevelLogic().bomba->render();
	}
	
	renderStateDisableZWriting( );
	renderStateDisableZTest( );
}

void SecondLevelState::renderWorld2D( ) {
	renderStateEnableAlphaBlending( );
	renderStateEnableZTest( );

	// Render Cilindros
	renderStateDisableCulling();
	cilindroNoradrenalina->render();
	cilindroSerotonina->render();
	renderStateEnableCulling();
	
	getSecondLevelLogic( ).power_ups.render( );

	// Render Particles
	PSManager::get( )->render( );

	renderStateDisableZTest( );
	renderStateDisableAlphaBlending( );
}

void SecondLevelState::renderPostProcessed2D( ) {
	renderStateEnableAlphaBlending( );

	vignetting.render( );

	if( hurt_time > 0.0f )
		hurt.render( hurt_time/max_hurt_time );

	renderStateDisableAlphaBlending( );
}

void SecondLevelState::renderGameUI( ) {
	renderStateEnableAlphaBlending( );

	second_level_ui.render( );

	renderStateDisableAlphaBlending( );
}

void SecondLevelState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 170;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0,  py, 0xffff0000, "Second Level Status");
	py += 10;

	py += printf(0, py, 0xffff0000, "Num PowerUps %d", getSecondLevelLogic( ).power_ups.size( ) );
#endif
}

void SecondLevelState::onLoad( ) {
	second_level_ui.reset( );
	if( getSecondLevelLogic( ).must_show_welcome_image ) {
		getSecondLevelLogic( ).must_show_welcome_image = false;
		second_level_ui.showWelcomeImage( );
	}

	hurt_time = 0.0f;
}

void SecondLevelState::onUnload( ) {
}

void SecondLevelState::loadGameState( ) {
	// Vignetting
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		vignetting.SETMATERIAL("vignetting_pan.dds", "shader.fx", "dummy_tech");
	else
		vignetting.SETMATERIAL("vignetting.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	vignetting.setMesh(mesh);

	// Hurt
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		hurt.load("hurt_pan.dds");
	else
		hurt.load("hurt.dds");
	D3DXMATRIX hurt_transform;
	D3DXMatrixScaling( &hurt_transform, aspect_ratio, 1.0f, 1.0f );
	hurt.setTransform( hurt_transform );

	second_level_ui.load( );

	TextureFactory::get()->createTexture("brain2_1.dds");
	TextureFactory::get()->createTexture("brain2_2.dds");
	TextureFactory::get()->createTexture("brain2_3.dds");
	TextureFactory::get()->createTexture("brain2_4.dds");
	TextureFactory::get()->createTexture("brain2_5.dds");
	TextureFactory::get()->createTexture("brain2_6.dds");

	cilindroSerotonina = CrearCilindro("recipiente.mesh", "cilindro_blue.dds",1.4666f, -1.345f);
	cilindroNoradrenalina = CrearCilindro("recipiente.mesh", "cilindro_red.dds", 4.91f, -1.26f);
}

Object * SecondLevelState::CrearCilindro(const std::string &mesh_name, const std::string &texture, float alpha, float delta)
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
