#ifndef FRUSTUM_H
#define FRUSTUM_H

struct BoundingSphere;

/**
* Generic Frustum to avoid drawing anything that is
* outside of the vision of camera
*/

class Frustum {
protected:
	float frustum[6][4];

public:	
	Frustum();
	~Frustum();

	void update();

	bool isBoundingVisible( const BoundingSphere &bsphere ) const;
};

#endif //FRUSTUM_H
