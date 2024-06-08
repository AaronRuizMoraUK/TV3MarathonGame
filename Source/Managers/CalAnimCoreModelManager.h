#ifndef CAL_CORE_ANIM_MODEL_MANAGER_H
#define CAL_CORE_ANIM_MODEL_MANAGER_H

#include "XMLParser/XMLParser.h"
#include "Object/AnimatedObject/CalAnimModel.h"

/**
* Factory of cal core models.
*
* This is a singleton.
*/

class CalAnimCoreModelManager : public CXMLParser 
{
protected:
	static std::string pathToCal;

	static const size_t max_cores = 5;
	CalAnimCoreModel *cores[ max_cores ];
	size_t  num_cores;

public:
	~CalAnimCoreModelManager();
	
	static CalAnimCoreModelManager * get();

	void load( );
	void onStartElement (const std::string &elem, MKeyValue &atts);
	void reload( );

	const CalAnimCoreModel * getCalAnimCoreModel( const std::string &filename ) const;

private:
	static CalAnimCoreModelManager * calAnimCoreModelFactory;
	CalAnimCoreModelManager();

};

#endif //CAL_CORE_ANIM_MODEL_MANAGER_H
