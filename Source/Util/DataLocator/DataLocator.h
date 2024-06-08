#ifndef DATA_LOCATOR_MANAGER_H
#define DATA_LOCATOR_MANAGER_H

#include <map>
#include "XMLParser/XMLParser.h"

/**
* Manager with information about data paths.
*/

class DataLocator : public CXMLParser
{
protected:
	 typedef std::map<std::string, std::string> Paths;
	 Paths paths;

public:
	~DataLocator();

	void initiate();
	void onStartElement (const std::string &elem, MKeyValue &atts);

	const std::string & getPath(const std::string &elem);
};

#endif //DATA_LOCATOR_MANAGER_H
