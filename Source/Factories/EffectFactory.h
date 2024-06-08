#ifndef EFFECT_FACTORY_H
#define EFFECT_FACTORY_H

#include "Directx/Directx.h"

/**
* Factory of effects (shaders).
*
* This is a singleton.
*/

class EffectFactory
{
protected:
	static std::string pathToShaders;

	static const size_t max_effects = 15;
	Effect effects[ max_effects ];
	size_t num_effects;

public:
	~EffectFactory();

	static EffectFactory * get();

	const Effect *createEffect(const std::string &filename);
	void reload( );

private:
	static EffectFactory * effectFactory;
	EffectFactory();

	const std::string &getRealPathFilename( const std::string &filename, const std::string &quality );
	Effect *findEffect( const std::string &filename );

	bool loadEffect(const std::string &filename, EffectDX &effect);
};

#endif //EFFECT_FACTORY_H
