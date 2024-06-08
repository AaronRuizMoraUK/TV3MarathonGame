#include "Bounding.h"
#include "Global/Rendering.h"
#include "Global/Geometry.h"

bool BoundingBox::isPointInside( const D3DXVECTOR3 &point ) const {
	return min_point < point && point < max_point;
}

void BoundingBox::renderDebug( ) const {
#ifndef NDEBUG
	D3DXVECTOR3 zero(min_point);
	D3DXVECTOR3 one(min_point.x, min_point.y, max_point.z);
	D3DXVECTOR3 two(min_point.x, max_point.y, min_point.z);
	D3DXVECTOR3 three(min_point.x, max_point.y, max_point.z);
	D3DXVECTOR3 four(max_point.x, min_point.y, min_point.z);
	D3DXVECTOR3 five(max_point.x, min_point.y, max_point.z);
	D3DXVECTOR3 six(max_point.x, max_point.y, min_point.z);
	D3DXVECTOR3 seven(max_point);

	drawLine(zero,one,0xffff0000);
	drawLine(one,three,0xffff0000);
	drawLine(three,two,0xffff0000);
	drawLine(two,zero,0xffff0000);
	drawLine(four,five,0xffff0000);
	drawLine(five,seven,0xffff0000);
	drawLine(seven,six,0xffff0000);
	drawLine(six,four,0xffff0000);
	drawLine(zero,four,0xffff0000);
	drawLine(two,six,0xffff0000);
	drawLine(three,seven,0xffff0000);
	drawLine(one,five,0xffff0000);
#endif
}

// --------------------------------------------------------

bool BoundingSphere::isPointInside( const D3DXVECTOR3 &point ) const {
	return distance( center, point ) < radius;
}

void BoundingSphere::renderDebug( ) const {
#ifndef NDEBUG
	D3DXVECTOR3 firstPoint(center.x-radius, center.y, center.z);
	D3DXVECTOR3 secondPoint(center.x+radius, center.y, center.z);
	drawLine(firstPoint, secondPoint,0xffffffff);

	D3DXVECTOR3 thirdPoint(center.x, center.y-radius, center.z);
	D3DXVECTOR3 fourthPoint(center.x, center.y+radius, center.z);
	drawLine(thirdPoint, fourthPoint,0xffffffff);

	D3DXVECTOR3 fifthPoint(center.x, center.y, center.z-radius);
	D3DXVECTOR3 sixthPoint(center.x, center.y, center.z+radius);
	drawLine(fifthPoint, sixthPoint,0xffffffff);
#endif
}
