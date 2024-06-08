#ifndef PLANET_MAP_H
#define PLANET_MAP_H

#include <string>

template<class Type>
class PlanetMap {
protected:
	std::string name;
	static std::string pathToPlanetMaps;

	short mapHeight;
	short mapWidth;
	Type *map;

public:
	PlanetMap( );
	~PlanetMap( );

	bool load( const std::string &aname );
	void destroy( );

	const std::string &getName( ) const { return name; }
	const Type &getData( float alpha, float delta ) const; // alpha = [0, 2PI], delta = [-PI/2, PI/2]
	const Type &getData( int x, int y ) const;
};

#include "PlanetMap.Impl.h"

#endif //PLANET_MAP_H
