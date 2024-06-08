#include <cassert>
#include "DataLocator.h"
#include "Global/Rendering.h"

DataLocator::~DataLocator() {
	printDebugString("Destroying DataLocator...\n");
	paths.clear();
};

void DataLocator::initiate()
{
	bool ok = xmlParseFile("./data/data_location.xml");
	assert(ok);
}

void DataLocator::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( elem == "data" )
	{
		const std::string &dataType = atts["type"];
		const std::string &dataPath = atts["path"];
		assert( !dataType.empty( ) );
		assert( !dataPath.empty( ) );

		paths[dataType] = dataPath;
	}
}

std::string empty = "";
const std::string & DataLocator::getPath(const std::string &elem)
{
	Paths::const_iterator it = paths.find(elem);

	if( it != paths.end() )
		return it->second;
	else {
		assert( !"Element not fount in DataLocator" );
		return empty;
	}
}
