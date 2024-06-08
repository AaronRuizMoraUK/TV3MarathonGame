#ifndef PLANET_MAP_MANAGER_H
#define PLANET_MAP_MANAGER_H

#include <cassert>
#include "Object/Planet/PlanetMap.h"
#include "XMLParser/XMLParser.h"
#include "Global/Rendering.h"

/**
* Manager of planet maps.
*
* This is a singleton.
*/

class PlanetMapManager : public CXMLParser {

protected:

	template<class Type>
	struct PlanetMaps {
		static const size_t max_maps = 5;
		PlanetMap<Type> maps[ max_maps ];
		size_t  num_maps;

		PlanetMaps( ) : num_maps( 0 ) {}
		~PlanetMaps( ) {
			printDebugString("Destroying PlanetMaps...\n");
		}

		PlanetMap<Type> * load( const std::string &filename ) {
			assert( num_maps < max_maps );
			size_t last_map = num_maps;
			bool ok = maps[ last_map ].load( filename );
			assert( ok );
			// Increment num maps
			++num_maps;
			return &maps[ last_map ];
		}

		void reload( ) {
			for( size_t i=0; i<num_maps; ++i ) {
				std::string name = maps[ i ].getName( ); // Necesario copiarlo, porque pasarse a sí mismo  para modificarlo no saldría bien
				bool ok = maps[ i ].load( name );
				assert( ok );
			}
		}

		const PlanetMap<Type> * findMap( const std::string &filename ) const {
			for( size_t i=0; i<num_maps; ++i ) {
				if( maps[ i ].getName( ) == filename )
					return &maps[ i ];
			}
			assert( !"Planet map not found" );
			return NULL;
		}
	};

	PlanetMaps<float>       heightmaps;
	PlanetMaps<D3DXVECTOR3> normalmaps;
	PlanetMaps<int>         texturemaps;

public:
	~PlanetMapManager() {
		printDebugString("Destroying PlanetMapManager...\n");
	}

	static PlanetMapManager * get();

	void load( );
	void onStartElement( const std::string &elem, MKeyValue &atts );
	void reload( );

	const PlanetMap<float> *      getHeightMap(const std::string &filename) const { return heightmaps.findMap( filename ); }
	const PlanetMap<D3DXVECTOR3> *getNormalMap(const std::string &filename) const { return normalmaps.findMap( filename ); }
	const PlanetMap<int> *        getTextureMap(const std::string &filename) const { return texturemaps.findMap( filename ); }

private:
	static PlanetMapManager * planetMapManager;
	PlanetMapManager() {}
};

#endif //PLANET_MAP_MANAGER_H
