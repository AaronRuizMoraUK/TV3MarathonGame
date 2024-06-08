#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Directx/Directx.h"

void anglesToVector(float yaw, float pitch, D3DXVECTOR3 &vector);
void vectorToAngles(const D3DXVECTOR3 &vector, float &yawOut, float &pitchOut);

inline float deg2rad( float degrees );
inline float rad2deg( float radians );

bool isFixedMatrix(const D3DXMATRIX &matrix);
void fixMatrix(D3DXMATRIX &dst, const D3DXMATRIX &src);

inline float distanceSq(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &target);
inline float distance(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &target);

void rotateOnAxis( D3DXMATRIX &output, const D3DXMATRIX &input, const D3DXVECTOR3 &axis, float angle );
bool obtainFrontPlayerUpScreen( D3DXVECTOR3 &out );

void normalizeVector3(D3DXVECTOR3 &dst, const D3DXVECTOR3 &src);
void normalize(float &dst, float src, float minNorm, float maxNorm, float min, float max);
void normalize(D3DXVECTOR2 &dst, const D3DXVECTOR2 &src, float minNorm, float maxNorm,
							 float min, float max);

void formatColorVector4ToDword(DWORD &color, const D3DXVECTOR4 &statusColor);
void formatColorDWordToVector4(D3DXVECTOR4 &statusColor, DWORD color);

float obtainRandom(float min, float max);

float clamp(float value, float min, float max);
int clamp(int value, int min, int max);

inline void lerp(D3DXVECTOR3 &dst, const D3DXVECTOR3 &src0, const D3DXVECTOR3 &src1, float t);
void lerp(D3DXQUATERNION &dst, const D3DXQUATERNION &src0, const D3DXQUATERNION &src1, float t);
void rotateVector3Quaternion( D3DXVECTOR3 &dst, const D3DXVECTOR3 &vector3,
														 const D3DXQUATERNION &quaternion );

#endif //GEOMETRY_H
