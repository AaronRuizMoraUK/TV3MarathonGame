#ifndef DOMAIN_GENERATOR_H
#define DOMAIN_GENERATOR_H

#include <map>
#include <stack>
#include <string>
#include "Papi/pAPI.h"

class MKeyValue;

class DomainGenerator {
public:
	DomainGenerator(void);
	~DomainGenerator(void);
	void clear(void);
	void pushDomain(MKeyValue &atts); 
	PAPI::pDomain * popDomain(void);
private:
	std::map<std::string, int> nameDomain;
	std::stack<PAPI::pDomain *> domainVolumes;
	std::stack<PAPI::pDomain *> toDelete;
};


#endif