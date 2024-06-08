#include "FinalScoresState.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"
#include "IO/IOStatus.h"
#include "Game/Game.h"
#include "Factories/QuadFactory.h"
#include "Logic/Logic.h"

FinalScoresState::FinalScoresState()
: enter_scores(true)
, exit_scores(false)
{
}

FinalScoresState::~FinalScoresState() {
	printDebugString("Destroying FinalScoresState...\n");
}

void FinalScoresState::update( float deltaTime ) {

	if ( enter_scores ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			enter_scores = false;
	}
	else if( exit_scores ) {
		if( !Global::fade.isFinished( ) ) {
			Global::fade.update( deltaTime );
		}
		else {
			// Go to final credits
			getState( "final_credits_state" )->getFocus( );
		}
	}
	else {
		if( ioStatus.start.becomesPressed( ) 
			|| ioStatus.selectOption.becomesPressed() )
			exit_scores = true;
	}
}

void FinalScoresState::preRender( ) {
}

void FinalScoresState::renderWorld3D( ) {
}

void FinalScoresState::renderWorld2D( ) {
}

void FinalScoresState::renderPostProcessed2D( ) {
	background.render();
}

void FinalScoresState::renderGameUI( ) {
	renderStateEnableAlphaBlending();

	// Render fade medium out
	if( enter_scores )
		Global::fade.renderFadeIn(0.5f);
	// Render fade in
	else if( exit_scores )
		Global::fade.renderFadeOut(0.5f);
	// Render score
	else {
		size_t num_score_infos = Global::score_recorder.getNumScoreInfos( );
		assert( num_score_infos > 0 );
		alphabet.printf( 0.33f, 0.8f, 0.07f, 0.07f, "puntuacions" );

		float py = 0.7f;
		for( size_t i=0; i<num_score_infos; ++i ) {
			const ScoreRecorder::ScoreInfo &score_info = Global::score_recorder.getScoreInfo( i );
			int day, month, year;
			score_info.getDate( day, month, year );

			std::string printf_char = "%2d) %s ";
			printf_char += PRINTF_SCORE_CHAR;
			printf_char += " (%02d-%02d-%04d)";
			alphabet.printf( 0.18f, py, 0.05f, 0.05f, printf_char.c_str(), i+1, score_info.initials, score_info.score, day, month, year );
			py -= 0.06f;
		}
	}

	renderStateDisableAlphaBlending();
}

void FinalScoresState::postRender( ) {
#ifndef NDEBUG
	// Print FPS
	int py = 0;
	py += printf(0, py, 0xffff0000, "Frames per second %0.2f", Game::fps);
	py += print(0, py, 0xffff0000, "Final Credits Status");
#endif
}

void FinalScoresState::onLoad( ) {
	enter_scores = true;
	exit_scores = false;
}

void FinalScoresState::onUnload( ) {
}

void FinalScoresState::loadGameState( ) {
	// Background
	float aspect_ratio = Game::configOptions.resolution.aspect_ratio;
	if( aspect_ratio != 4.0f/3.0f && aspect_ratio != 5.0f/4.0f )
		background.SETMATERIAL("top_score_pan.dds", "shader.fx", "dummy_tech");
	else
		background.SETMATERIAL("top_score.dds", "shader.fx", "dummy_tech");

	const IMesh * mesh = QuadFactory::get()->createQuad( 1.0f, 1.0f );
	assert(mesh);
	background.setMesh(mesh);

	alphabet.load( );
}
