#ifndef INSTRUCTIONS_STATE_H
#define INSTRUCTIONS_STATE_H

#include "GameState/IGameState.h"
#include "Util/ComicTextWindow/ComicTextWindow.h"
#include "Expat/XMLParser.h"
#include "Object/AnimatedObject/CalAnimatedObject.h"

class InstructionsState : public IGameState, public CXMLParser
{
	
public:
	enum InstructionsEnun {
		NONE = 0
		, GO_TO_MENU
		, GAME_OVER
		, SCORE_RECOUNT

		, INIT_FIRST_LEVEL
		, LOSE_FIRST_LEVEL
		, INIT_SECOND_LEVEL
		, LOSE_SECOND_LEVEL
		, INIT_THIRD_LEVEL
		, LOSE_THIRD_LEVEL
		, INIT_FOURTH_LEVEL
		, LOSE_FOURTH_LEVEL
		, GAME_WON

		, NUM_INSTRUCTIONS
	};
	static const char *instructions_chars[ NUM_INSTRUCTIONS ];
	InstructionsEnun instruction; // Current instruction

protected:
	static std::string pathToXml;

	enum StatusEnum {
		FADING_IN = 0
		, EXPLAINING
		, FADING_OUT
	};
	StatusEnum status;

	Object background;
	ComicTextWindow comic_text_window;
	CalAnimatedObject instructor;

	// Texts
	typedef std::map<InstructionsEnun, std::string> TInstructions;
	TInstructions instructions_texts;

	float ok_action_duration;
	float ok_curr_time;

	std::string idle_anim;
	std::string ok_anim;

	// Parpadeo ENTER msg
	static const float print_enter_time;
	float curr_print_enter_time;
	bool print_enter;

	// Sound
	DWORD sound_id;

public:
	InstructionsState();
	~InstructionsState();

	void onLoad( );
	void onUnload( );
	const char *getName( ) const { return "instructions_state"; }

	void onStartElement(const std::string &elem, MKeyValue &atts);

protected:
	void update( float deltaTime );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void loadGameState();

private:
	InstructionsEnun getInstruction( const char *inst_text ) const;
	void goToNextState( );

};

#endif // INSTRUCTIONS_STATE_H
