#ifndef CAL_ANIMATED_OBJECT_H
#define CAL_ANIMATED_OBJECT_H

#include "Object/AnimatedObject/AnimatedObject.h"
#include "CalAnimModel.h"

/**
* A cal3d animated object in scene.
*/

class CalAnimatedObject : public AnimatedObject
{
public:
	static bool cal_animation_enabled;

protected:
	CalAnimModel *calAnimModel;  // Este es el cal3d del objeto: playCycle, playAction...

	bool updateSkeleton;

public:
	CalAnimatedObject();
	CalAnimatedObject( const CalAnimatedObject &source );
	virtual ~CalAnimatedObject();
	virtual void operator=( const CalAnimatedObject &source );

	void updateAnimation(float deltaTime);
	void render();
	void renderDebug();

	CalAnimModel * getCalAnimModel( ) const { return calAnimModel; }
	void setCalAnimModel( const char *cal_core_model_name ); // Con esto se inicializa un modelo de cal3D

private:
	void setCalAnimModel( CalAnimModel *calAnimModel );

	void destroy( );
	void copy( const CalAnimatedObject &source );

};

#endif //CAL_ANIMATED_OBJECT_H
