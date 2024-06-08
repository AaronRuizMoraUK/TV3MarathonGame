#ifndef RIGID_ANIMATION_RESOURCE_H
#define RIGID_ANIMATION_RESOURCE_H

#include "Directx/Directx.h"

/**
* Resource of a keyframed prerecorded animation
*/

class RigidAnimationResource
{
protected:
    float duration;
            
    unsigned int numPositionSamples;
    D3DXVECTOR3 *positions;
    
    unsigned int numRotationSamples;
    D3DXQUATERNION *rotations;

public:
    RigidAnimationResource();
    ~RigidAnimationResource();

    // Initialization Methods
    // Builds a constant channel (with 1 position and 1 rotation frames)
    void initConstant( const D3DXVECTOR3 &pPosition, const D3DXQUATERNION &pRotation, float pDuration );
    void initLinear( const D3DXVECTOR3 &pPosition0, const D3DXQUATERNION &pRotation0,
                     const D3DXVECTOR3 &pPosition1, const D3DXQUATERNION &pRotation1,
                     float pDuration );
    bool initFromFile( const char *file );

    // Sample Retrieval methods   
    float getDuration() const { return duration; }
    unsigned int getNumPositionSamples() const { return numPositionSamples; }
    unsigned int getNumRotationSamples() const { return numRotationSamples; }
    void getPositionSample( unsigned int sampleIndex, D3DXVECTOR3 &pPosition ) const;
    void getRotationSample( unsigned int sampleIndex, D3DXQUATERNION &pRotation ) const;
    
    // Continuous-time methods        
    void getPosition( float time, D3DXVECTOR3 &pPosition ) const;
    void getRotation( float time, D3DXQUATERNION &pRotation ) const;         
    
private:
    void init( float pDuration, unsigned int pNumPositionSamples, unsigned int pNumRotationSamples );
    void clear();
    float time2Lambda( float time ) const { return (time/duration); }
};

struct ROAResource {
	std::string name;
	RigidAnimationResource rigidAnimationResource;

	ROAResource( ) : name( "" ) {}
};

#endif // RIGID_ANIMATION_RESOURCE_H
