#ifndef CAL_ANIM_MODEL_H
#define CAL_ANIM_MODEL_H

#include "cal3d/cal3d.h"
#include "XMLParser/XMLParser.h"

/**
* This struct has cal3d core information and all the ids
* of the animations and mesh
*/

struct CalAnimCoreModel : public CXMLParser {
public:
	std::string name;
	static std::string pathToCal;

	// Cal3D core model
	CalCoreModel calCoreModel;

	// Cal3D mesh Id
	int meshId;

	// Core Animations
	struct CoreAnim {
		int   id;
		float def_delay_in, def_delay_out;
		float def_time_factor;
		const CalCoreAnimation *cal_core_animation; // Pertenece a calCoreModel, ya se destruye junto con él
		CoreAnim( )
			: id( -1 )
			, def_delay_in( 0.0f )
			, def_delay_out( 0.0f )
			, def_time_factor( 1.0f )
			, cal_core_animation( NULL )
		{}
	};
	typedef std::map<std::string, CoreAnim> CoreAnimMap;
	CoreAnimMap coreAnimMap;

private:
	// Just for reload time factor
	bool reload_time_factors;

public:
	CalAnimCoreModel( const std::string &aname );
	~CalAnimCoreModel( );

	void onStartElement(const std::string &elem, MKeyValue &atts);
	void reloadTimeFactors( );

	const CoreAnim &getCoreAnim( const std::string &anim_name ) const;

private:
	CoreAnim &getCoreAnim( const std::string &anim_name );
};

// ------------------------------------------------------------------------

/**
* This struct has cal3d information and the core
*/

struct CalAnimModel {
public:
	// Cal3D model (instancia)
	CalModel calModel;

	// CORE (manager)
	const CalAnimCoreModel *core; // Pertenece al CalAnimCoreModelManager, lo destruye él

public:
	CalAnimModel( const CalAnimCoreModel *acore );
	CalAnimModel( const CalAnimModel &source );
	~CalAnimModel();
	void operator=( const CalAnimModel &source );

	// NOTA: Si no se pone delay, coge los que tenga por defecto el core de la
	//       animación (será lo más común)

	// Elimina todos los ciclos e introduce uno (es lo normal con los ciclos, sólo queremos uno)
	void playCycle(const std::string &name, float delay = -1.0f);

	// Reproduce una acción (se quita sola). Devuelte la duración de la acción.
  float playAction(const std::string &name, bool autoLock = false, float delayIn = -1.0f, float delayOut = -1.0f);
	// Elimina una acción (la interrumpe)
	void removeAction(const std::string &name, float delay = -1.0f);

	// Limpiar con o sinf delay
	void clearAllCycles( float delay = 0.0f );
	void clearAllActions( float delay = 0.0f );
	void clearAllAnimations( float delay = 0.0f );

private:
	void destroy( );
	
	// Reproduce  y elimina un ciclo
  void blendCycle(const std::string &name, float delay = -1.0f);
  void clearCycle(const std::string &name, float delay = -1.0f);

	void blendCycle(int id, float time_factor, float delay);
	CalAnimationAction *getActionInMixer( const CalCoreAnimation *cal_core_animation );
};

#endif //CAL_ANIM_MODEL_H
