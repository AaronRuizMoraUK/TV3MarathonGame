#ifndef ROA_RESOURCE_MANAGER_H
#define ROA_RESOURCE_MANAGER_H

#include "Expat/XMLParser.h"
#include "Controllers/ROAResource.h"

/**
* Manager of ROA Resources.
*
* This is a singleton.
*/

class ROAResourceManager : public CXMLParser {
protected:
	static std::string pathToROAS;

	static const size_t max_resources = 5;
	ROAResource resources[ max_resources ];
	size_t  num_resources;

public:
	~ROAResourceManager();

	static ROAResourceManager * get();

	void load( );
	void onStartElement( const std::string &elem, MKeyValue &atts );
	void reload( );

	const ROAResource *getResource( const std::string &filename ) const;

private:
	static ROAResourceManager * roaResourceManager;
	ROAResourceManager();
};

#endif //ROA_RESOURCE_MANAGER_H
