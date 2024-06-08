#include <cassert>
#include "RenderTexture.h"
#include "Global/Rendering.h"
#include "Global/Variables.h"

RenderTexture::RenderTexture( )
: surface( NULL )
, texture( NULL )
, colorFormat( D3DFMT_A8R8G8B8 )
, renderToSurface( NULL )
, stencilZBufferFormat( D3DFMT_D24S8 )
, useDepthBuffer( false )
, width( 0 )
, height( 0 )
, initiated( false )
{
}

RenderTexture::~RenderTexture( ) {
	printDebugString("Destroying RenderTexture...\n");

	destroy( );
}

bool RenderTexture::create( int pWidth, int pHeight, D3DFORMAT pColorFormat, D3DFORMAT pStencilZBufferFormat ) {
	assert( !initiated );

	width = pWidth;
	height = pHeight;
	colorFormat = pColorFormat;
	stencilZBufferFormat = pStencilZBufferFormat;
	
	HRESULT hr;

	// Create Texture
	hr = D3DXCreateTexture( Global::device
		, width, height
		, 1                         // mip levels
		, D3DUSAGE_RENDERTARGET
		, colorFormat
		, D3DPOOL_DEFAULT
		, &texture);
	texture_bypass.tex = texture;
	if(hr != D3D_OK) {
		return false;
	}

	// Obtain the surface
	hr = texture->GetSurfaceLevel(0, &surface);
	assert (hr == D3D_OK);

	// Obtain the surface description
	SurfaceDescription desc;
	hr = surface->GetDesc(&desc);
	assert (hr == D3D_OK);

	// Create the render to surface
	useDepthBuffer = ( stencilZBufferFormat != D3DFMT_UNKNOWN );
	hr = D3DXCreateRenderToSurface( Global::device,
		desc.Width, desc.Height,
		desc.Format,
		useDepthBuffer,
		stencilZBufferFormat,
		&renderToSurface );

	initiated = (hr == D3D_OK);
	return initiated;
}

void  RenderTexture::destroy( ) {
	initiated = false;

	// Destroy Render to Surface
	if( renderToSurface != NULL )
		renderToSurface->Release(), renderToSurface = NULL;

	// Destroy Surface
	if( surface != NULL )
		surface->Release(), surface = NULL;

	// Destroy Texture
	if( texture != NULL )
		texture->Release(), texture = NULL;
	texture_bypass.tex = NULL;
}

void RenderTexture::begin( ) {
	assert( initiated );

	HRESULT hr = renderToSurface->BeginScene( surface, NULL );
	assert (hr == D3D_OK);
}

void RenderTexture::end( ) {
	assert( initiated );

	HRESULT hr = renderToSurface->EndScene( 0 );

	assert (hr == D3D_OK);
}

void RenderTexture::save( const std::string &filename, D3DXIMAGE_FILEFORMAT format ) const {
	assert( initiated && texture );
	assert( format!=D3DXIFF_TGA && format!= D3DXIFF_PPM );

	std::string fullFilename = filename;
	
	if( format == D3DXIFF_JPG )
		fullFilename += ".jpg";
	else if( format == D3DXIFF_DDS )
		fullFilename += ".dds";
	else if( format == D3DXIFF_BMP )
		fullFilename += ".bmp";
	else {
		assert( !"Format not supported in RenderTexture" );
		return;
	}

	// Save texture to a file in DDS format
	HRESULT hr = D3DXSaveTextureToFile( fullFilename.c_str(), format, texture, NULL );

	assert (hr == D3D_OK);
}

// ----------------------------------------- //
// Create a texture, write on it and save it //
// ----------------------------------------- //

/*
bool readHeightMap (const std::string &filename, float ***heightMap, short &height, short &width) {

	// Open file
	std::ifstream myFile;
	myFile.open(filename.c_str(), std::ios::in | std::ios::binary);

	if (myFile.is_open()) {

		// Read height
		if ( !myFile.read((char *)&height, 2) )
			return false;

		// Read Width
		if ( !myFile.read((char *)&width, 2) )
			return false;

		// Check dimensions
		if( height<=1 || width<=1 )
			return false;

		// Reserve memory
		(*heightMap) = new float *[height];
		for(int i=0; i<height; ++i)
			(*heightMap)[i] = new float[width];

		// Read heights the texture
		for(int y=0; y<height; ++y) {
			for(int x=0; x<width; ++x) {
				if ( !myFile.read((char *)&((*heightMap)[y][x]), 4) )
					return false;
			}
		}

		// Close file
		myFile.close();

		return true;
	}
	else
		return false;
}

void createHeightMapTexture() {
	// Texture
	Texture heightMapTexture = NULL;

	// Heightmap information
	short height = 0;
	short width = 0;
	float **heightMap = NULL;

	// Read heightmap from file
	std::string filename = "./data/iuranus_surface_heightmap.bin";
	bool ok = readHeightMap(filename, &heightMap, height, width);
	if( !ok )
		return;

	HRESULT hResult;
	// Create the texture
	hResult = D3DXCreateTexture( Global::device, width, height, 1, 0, D3DFMT_R32F,
		D3DPOOL_SCRATCH, &heightMapTexture);
	if( FAILED(hResult) )
		return;

	// Lock
	D3DLOCKED_RECT lr;
	hResult = heightMapTexture->LockRect( 0, &lr, NULL, 0);
	if( FAILED(hResult) )
		return;

	// Obtain a pointer to the texture data
	float *pBitmap = (float *) lr.pBits;

	// Fill the texture
	for(int y=0; y<height; ++y) {
		//char msg[ 10000 ];
		//sprintf_s(msg, sizeof(msg), "");
		for(int x=0; x<width; ++x) {
			//char msgTmp[1000];
			//sprintf_s(msgTmp, sizeof(msgTmp), "%f ", heightMap[y][x]);
			//strcat_s( msg, sizeof(msg), msgTmp);

			// Height in RGB
			//D3DXVECTOR3 heightRGB(0,0,255);

			// Height in texture format
			//DWORD height;
			//Global::formatColorVector4ToDword(height, D3DXVECTOR4(heightRGB, 0));

			// Write height into texture
			//pBitmap[width*y + x] = height;
			pBitmap[width*y + x] = heightMap[y][x];

		}
		//strcat_s( msg, sizeof(msg), "\n" );
		//printDebugString(msg);
	}

	// Unlock
	hResult = heightMapTexture->UnlockRect( 0 );
	if( FAILED(hResult) )
		return;

	// Save texture to file
	std::string filenameOutput = "./data/iuranus_surface_heightmap.dds";
	hResult = D3DXSaveTextureToFile(filenameOutput.c_str(), D3DXIFF_DDS, heightMapTexture, NULL);
	if( FAILED(hResult) )
		return;

	// Destroy heightMap
	if(heightMap!=NULL) {
		for(int i=0; i<height; ++i)
			delete [] heightMap[i];
		delete [] heightMap;
		heightMap = NULL;
	}

	// Destroy Texture
	if(heightMapTexture!=NULL)
		heightMapTexture->Release(), heightMapTexture = NULL;
}

void readHeightMapTexture() {
	// Texture
	Texture heightMapTexture = NULL;

	// Read heightmap from file
	std::string filename = "iuranus_surface_heightmap.dds";
	std::string filenamePath = "./data/" + filename;

	D3DXIMAGE_INFO textureInfo;

	HRESULT hResult;
	// Create the texture
	hResult = D3DXCreateTextureFromFileEx( Global::device, filenamePath.c_str(), 0, 0, 1, 0,
		D3DFMT_R32F, D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR,
		D3DX_FILTER_NONE, 0, &textureInfo, NULL, &heightMapTexture);
	if( FAILED(hResult) )
		return;

	// Add the texture to texture factory
	TextureFactory::getTextureFactory()->addTexture(filename, heightMapTexture);

	// Destroy Texture
	//if(heightMapTexture!=NULL)
	//heightMapTexture->Release(), heightMapTexture = NULL;
}
*/
