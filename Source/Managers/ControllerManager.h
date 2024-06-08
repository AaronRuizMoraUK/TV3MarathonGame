#ifndef CONTROLLER_MANAGER_H
#define CONTROLLER_MANAGER_H

#include <cassert>
#include "Expat/XMLParser.h"
#include "AIControllers/IAIController.h"
#include "AnimationControllers/IAnimationController.h"
#include "PlayerControllers/IPlayerController.h"
#include "CameraControllers/ICameraController.h"

/**
* Manager of controller definitions.
*
* This is a singleton.
*/

class ControllerManager : public CXMLParser {

protected:
	static std::string pathXML;

	// Template of controllers
	template<class T>
	class Controllers {
	protected:
		struct Controller {
			std::string name;
			T *controller;
			Controller( ) : name( "" ), controller( NULL ) {}
		};

		static const size_t max_controllers = 10;
		Controller controllers[ max_controllers ];
		size_t  num_controllers;

	public:
		Controllers( ) : num_controllers( 0 ) {}
		virtual ~Controllers( ) { destroy( ); }

		void destroy( ) {
			for( size_t i=0; i<num_controllers; ++i ) {
				controllers[ i ].name = "";
				if( controllers[ i ].controller )
					delete controllers[ i ].controller, controllers[ i ].controller = NULL;
			}
			num_controllers = 0;
		}

		void load( MKeyValue &atts ) {
			const std::string &name = atts["name"];
			T *new_controller = loadController( name, atts );
			add( name, new_controller );
		}

		T *create( const std::string &aname ) const {
			const Controller* controller = find( aname );
			if( controller == NULL || controller->controller == NULL )
				return NULL;
			return createController( controller );
		}

	protected:
		void add( const std::string aname, T *new_controller ) {
			if( new_controller == NULL )
				return;

			// Add new controller
			assert( num_controllers < max_controllers );
			size_t last_controller = num_controllers;
			controllers[ last_controller ].name = aname;
			assert( controllers[ last_controller ].controller == NULL );
			controllers[ last_controller ].controller = new_controller;

			// Increment num controllers
			++num_controllers;
		}

		const Controller *find( const std::string &aname ) const {
			for( size_t i=0; i<num_controllers; ++i ) {
				if( controllers[ i ].name == aname )
					return &controllers[ i ];
			}
			return NULL;
		}

		virtual T *loadController( const std::string &name, MKeyValue &atts ) = 0;
		virtual T *createController( const Controller *controller ) const = 0;
	};

	class AIControllers : public Controllers<IAIController> {
	protected:
		IAIController *loadController( const std::string &name, MKeyValue &atts );
		IAIController *createController( const Controller *controller ) const;
	};

	class AnimationControllers : public Controllers<IAnimationController> {
	protected:
		IAnimationController *loadController( const std::string &name, MKeyValue &atts );
		IAnimationController *createController( const Controller *controller ) const;
	};

	class PlayerControllers : public Controllers<IPlayerController> {
	protected:
		IPlayerController *loadController( const std::string &name, MKeyValue &atts );
		IPlayerController *createController( const Controller *controller ) const;
	};

	class CameraControllers : public Controllers<ICameraController> {
	protected:
		ICameraController *loadController( const std::string &name, MKeyValue &atts );
		ICameraController *createController( const Controller *controller ) const;
	};

	// ---------------------------------------------------------------
	AIControllers        ai_controllers;
	AnimationControllers animation_controllers;
	PlayerControllers    player_controllers;
	CameraControllers    camera_controllers;

public:
	~ControllerManager();

	static ControllerManager * get();

	void load( );
	void onStartElement( const std::string &elem, MKeyValue &atts );
	//void reload( );

	// Devuelven una nueva instancia (un new, que pasará a ser propiedad de los Object)
	IAIController *       getAIController(        const std::string &name ) const { return ai_controllers.create( name ); }
	IAnimationController *getAnimationController( const std::string &name ) const { return animation_controllers.create( name ); }
	IPlayerController *   getPlayerController(    const std::string &name ) const { return player_controllers.create( name ); }
	ICameraController *   getCameraController(    const std::string &name ) const { return camera_controllers.create( name ); }

private:
	static ControllerManager * controllerManager;
	ControllerManager() {}
};

#endif //CONTROLLER_MANAGER_H
