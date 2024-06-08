#ifndef ACTION_PS_MANAGER_H
#define ACTION_PS_MANAGER_H

#include <string>
#include <stack>
#include <queue>
#include <map>
#include "Papi/pAPI.h"

class MKeyValue;
class DomainGenerator;

class ActionPSManager {
public:
	ActionPSManager(void);
	~ActionPSManager(void);
	
	void enterAction(const std::string &action, MKeyValue &atts); 
	void exitAction(const std::string &action, DomainGenerator &dom, PAPI::ParticleContext_t &particleEngine);

	inline void addNewFunction(void) { functions.push(new std::queue<float>); }
	void addSample(MKeyValue &atts);

private:
	std::map<std::string, int> nameDomain;
	std::stack<float> parametersF;
	std::stack<bool> parametersB;
	std::stack< std::queue<float> * > functions;

	float getFloatValue(void);
	bool getBoolValue(void);
	PAPI::pVec getVectorValue(DomainGenerator &dom);
	PAPI::pDomain * getDomainValue(DomainGenerator &dom);
	std::queue<float> * getFunctionValue(void);
};


#endif