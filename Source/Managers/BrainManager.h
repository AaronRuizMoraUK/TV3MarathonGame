#ifndef BRAIN_MANAGER_H
#define BRAIN_MANAGER_H

#include "Expat/XMLParser.h"
#include "Object/Planet/Planet.h"

/**
* Manager of brains.
*
* This is a singleton.
*/

class BrainManager : public CXMLParser {

protected:
	static const size_t max_brains = 4;
	Planet  brains[ max_brains ];
	size_t  num_brains;

	size_t  current_brain;

public:
	~BrainManager();

	static BrainManager * get();

	void load( );
	void onStartElement( const std::string &elem, MKeyValue &atts );

	Planet &getCurrentBrain( );
	void setCurrentBrain( size_t index ); // [1,n]

private:
	static BrainManager * brainManager;
	BrainManager();
};

#endif //BRAIN_MANAGER_H
