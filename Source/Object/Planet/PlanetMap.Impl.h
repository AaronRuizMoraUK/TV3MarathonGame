#ifndef PLANET_MAP_IMPL_H
#define PLANET_MAP_IMPL_H

template<class Type>
std::string PlanetMap<Type>::pathToPlanetMaps = "";

template<class Type>
PlanetMap<Type>::PlanetMap( )
: name( "" )
, mapHeight( 0 )
, mapWidth( 0 )
, map( NULL )
{
}

template<class Type>
PlanetMap<Type>::~PlanetMap( ) {
	destroy( );
}

template<class Type>
bool PlanetMap<Type>::load( const std::string &aname ) {
	// Destroy map if exists
	destroy( );

	name = aname;
	if( pathToPlanetMaps.empty( ) )
		pathToPlanetMaps = Global::data_locator.getPath( "planetmap" );
	std::string fileString = pathToPlanetMaps + name;
	const char *file = fileString.c_str( );

	// Open file
	FILE *myFile = NULL;
	errno_t error = fopen_s( &myFile, file, "rb" );

	if( error != 0 || !myFile ) {
		assert( !"Heightmap file does not exists" );
		return false;
	}

	size_t size;
	size_t size_readed;

	size = sizeof( short );
	size_readed = fread( &mapHeight, sizeof( char ), size, myFile );
	assert( size == size_readed );

	size = sizeof( short );
	size_readed = fread( &mapWidth, sizeof( char ), size, myFile );
	assert( size == size_readed );

	// Check dimensions
	assert( mapHeight>1 && mapWidth>1 );

	size = mapHeight * mapWidth;
	map = new Type[ size ];
	assert( map );
	size_readed = fread( map, sizeof( Type ), size, myFile );
	printfDebugString( "Reading map %s [%d,%d] (%d/%d)\n", file
		, mapHeight, mapWidth
		, size_readed * sizeof( Type ), size * sizeof( Type ) );
	assert( size == size_readed );

	// Close file
	fclose( myFile );

	return true;
}

template<class Type>
void PlanetMap<Type>::destroy( ) {
	// Destroy map
	if( map )
		delete [] map, map = NULL;
	name = "";
	mapHeight = mapWidth = 0;
}

template<class Type>
const Type & PlanetMap<Type>::getData( float alpha, float delta ) const {
	if(map==NULL) {
		assert( !"Planet Map is NULL" );
		static Type zero;
		memset( &zero, 0x00, sizeof( zero ) );
		return zero;
	}

	// Normalize alpha and delta angles to height map matrix dimensions
	float coordDelta = 0.0f;
	float coordAlpha = 0.0f;
	normalize(coordDelta, delta, 0.0f, mapHeight-1.0f, -D3DX_PI/2.0f, D3DX_PI/2.0f);
	normalize(coordAlpha, alpha, 0.0f, mapWidth-1.0f, 0.0f, 2.0f*D3DX_PI);

	// Obtain the coordinates to access to heightmap matrix
	float intPart;
	float fracPart;

	int coorY;
	fracPart = modff(coordDelta, &intPart);
	if(fracPart>=0.5f)
		coorY = ((int)intPart)+1;
	else
		coorY = (int)intPart;

	int coorX;
	fracPart = modff(coordAlpha, &intPart);
	if(fracPart>=0.5f)
		coorX = ((int)intPart)+1;
	else
		coorX = (int)intPart;

	return getData( coorY, coorX );
}

template<class Type>
const Type & PlanetMap<Type>::getData( int x, int y ) const {
	if(map==NULL) {
		assert( !"Planet Map is NULL" );
		static Type zero;
		memset( &zero, 0x00, sizeof( zero ) );
		return zero;
	}

	return *(map + mapWidth*x + y); // map[x][y]
}

#endif //PLANET_MAP_H
