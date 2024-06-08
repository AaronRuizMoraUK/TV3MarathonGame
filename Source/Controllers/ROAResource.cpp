#include "ROAResource.h"
#include "Global/Geometry.h"
#include "Global/Rendering.h"

#define ENABLE_INTERPOLATION

RigidAnimationResource::RigidAnimationResource()
: duration(0)
, numPositionSamples(0)
, positions( NULL )
, numRotationSamples(0)
, rotations( NULL )
{}

RigidAnimationResource::~RigidAnimationResource()
{
	printDebugString("Destroying Rigid Animation Resource...\n");
	clear();
}

void RigidAnimationResource::init( float pDuration, unsigned int pNumPositionSamples, unsigned int pNumRotationSamples )
{
	clear();

	duration = pDuration;
	numPositionSamples = pNumPositionSamples;
	numRotationSamples = pNumRotationSamples;

	if( numPositionSamples > 0 )
		positions = new D3DXVECTOR3[numPositionSamples];
	if( numRotationSamples > 0 )           
		rotations = new D3DXQUATERNION[numRotationSamples];     
}

void RigidAnimationResource::clear()
{
	if( numPositionSamples != 0 )                
		delete []positions, positions=NULL;
	if( numRotationSamples != 0 )
		delete []rotations, rotations=NULL;

	duration = 0;
	numPositionSamples = 0;
	numRotationSamples = 0;
}

void RigidAnimationResource::initConstant( const D3DXVECTOR3 &pPosition, const D3DXQUATERNION &pRotation, float pDuration )
{
	init( pDuration, 1, 1 );
	positions[0] = pPosition;
	rotations[0] = pRotation;
}

void RigidAnimationResource::initLinear( const D3DXVECTOR3 &pPosition0, const D3DXQUATERNION &pRotation0,
                     const D3DXVECTOR3 &pPosition1, const D3DXQUATERNION &pRotation1,
                     float pDuration )
{
	init( pDuration, 2, 2 );
	positions[0] = pPosition0;
	rotations[0] = pRotation0;
	positions[1] = pPosition1;
	rotations[1] = pRotation1;
}

bool RigidAnimationResource::initFromFile( const char *file )
{
	FILE *pf;
	int hr = fopen_s( &pf, file, "rt" );
	if( hr!=0 )
		return false;

	char nodeName[128];
	int numSamples;
	float pDuration;

	fscanf_s( pf, "<RigidAnimation>\n" );
	fscanf_s( pf, "<Name> %s </Name>\n", nodeName, (unsigned int)sizeof(nodeName) );
	fscanf_s( pf, "<NumSamples> %d </NumSamples>\n", &numSamples );
	fscanf_s( pf, "<Duration> %f </Duration>\n", &pDuration );

	// Read track data
	init( pDuration, numSamples, numSamples );
	fscanf_s( pf, "<Position>\n" );
	for( int i=0; i<numSamples; ++i )
		fscanf_s( pf, "%f %f %f\n", &(positions[i].x), &(positions[i].y), &(positions[i].z) );
	fscanf_s( pf, "</Position>\n" );

	fscanf_s( pf, "<Orientation>\n" );
	for( int i=0; i<numSamples; ++i )
		fscanf_s( pf, "%f %f %f %f\n", &(rotations[i].x), &(rotations[i].y), &(rotations[i].z), &(rotations[i].w) );
	fscanf_s( pf, "</Orientation>\n" );

	fscanf_s( pf, "</RigidAnimation>" );

	fclose( pf );

	return true;
}

//---- Sample Retrieval methods
void RigidAnimationResource::getPositionSample( unsigned int sampleIndex, D3DXVECTOR3 &pPosition ) const
{        
	pPosition = positions[ sampleIndex ];
}

void RigidAnimationResource::getRotationSample( unsigned int sampleIndex, D3DXQUATERNION &pRotation ) const
{
	pRotation = rotations[ sampleIndex ];
}

//---- Continuous-time sampling methods
void RigidAnimationResource::getPosition( float time, D3DXVECTOR3 &pPosition ) const
{
#if !defined(ENABLE_INTERPOLATION)
	float lambda = time2Lambda( time );
	float clampedLambda = clamp(lambda, 0.0f, 1.0f);
	int frameIndex = (int) floorf( (numPositionSamples-1) * clampedLambda );
	pPosition = positions[ frameIndex ];
#else        
	float lambda = time2Lambda( time );
	float clampedLambda = clamp(lambda, 0.0f, 1.0f);
	float frameTime = (numPositionSamples-1) * clampedLambda;
	int frameIndex = (int) floorf( frameTime );
	float frameLambda = frameTime - frameIndex;
	if( frameLambda )
		lerp(pPosition, positions[frameIndex], positions[frameIndex+1], 1.0f-frameLambda);
	else
		pPosition = positions[ frameIndex ];
#endif        
}

void RigidAnimationResource::getRotation( float time, D3DXQUATERNION &pRotation ) const
{        
#if !defined(ENABLE_INTERPOLATION)
	float lambda = time2Lambda( time );
	float clampedLambda = clamp(lambda, 0.0f, 1.0f);
	int frameIndex = (int) floorf( (numRotationSamples-1) * clampedLambda );
	pRotation = rotations[ frameIndex ];
#else        
	float lambda = time2Lambda( time );
	float clampedLambda = clamp(lambda, 0.0f, 1.0f);
	float frameTime = (numRotationSamples-1) * clampedLambda;
	int frameIndex = (int) floorf( frameTime );
	float frameLambda = frameTime - frameIndex;
	if( frameLambda )
		lerp(pRotation, rotations[frameIndex], rotations[frameIndex+1], 1.0f-frameLambda);
		//D3DXQuaternionSlerp(&pRotation, &(rotations[frameIndex]), &(rotations[frameIndex+1]), 1.0f-frameLambda);
	else
		pRotation = rotations[ frameIndex ];
#endif         
	D3DXQuaternionNormalize(&pRotation, &pRotation);
}
