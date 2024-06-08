#ifndef CORESUBSYSTEM_H
#define CORESUBSYSTEM_H

#include <vector>
#include "Directx/Directx.h"

class CorePSubsystem {
private:
	struct TUpdateParams {
		int updateHandle;
		float initTime;
		float finishTime;
	};

public:
	CorePSubsystem(void);
	~CorePSubsystem(void); // DeleteActionList
	void destroy( );

	int getUpdateHandle(float lifeTime) const;
	inline int getMaxParticles(void) const			{ return maxParticles; }
	inline float getParticleColorFactor(void) const		{ return particleColorFactor; };
	inline const Texture *getTexture(void) const		{ return texture; }
	inline const std::string &getShaderType(void)	const			{ return shaderType; }

	void setUpdateHandle(int handle, float init = 0.0f, float finish = -1.0f);
	inline void setMaxParticles(int param)			{ maxParticles = param; }
	inline void setParticleColorFactor(float param)	{ particleColorFactor = param; }
	inline void setTexture(const Texture *param)		{ texture = param; }
	inline void setShaderType(const std::string &param)			{ shaderType = param; }

private:
	int maxParticles;
	std::string shaderType;
	float particleColorFactor;
	const Texture *texture;

	std::vector<TUpdateParams> updateParamsVector;
};

#endif