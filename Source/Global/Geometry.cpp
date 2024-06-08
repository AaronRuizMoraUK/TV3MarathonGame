#include <cassert>
#include "Directx/Directx.h"
#include "Variables.h"
#include "PlayerControllers/SpherePlayerController/SpherePlayerController.h"

void anglesToVector(float yaw, float pitch, D3DXVECTOR3 &vector) {
	float cosf_pitch = cosf( pitch );

	vector.x = sinf(yaw)*cosf_pitch;
	vector.y = -sinf(pitch);
	vector.z = cosf(yaw)*cosf_pitch;
}

void vectorToAngles(const D3DXVECTOR3 &vector, float &yawOut, float &pitchOut) {
	yawOut = atan2f(vector.x, vector.z);
	pitchOut = -atan2f( vector.y, sqrtf( powf(vector.x, 2) + powf(vector.z, 2)) );
}

float deg2rad( float degrees ) {
	return degrees * D3DX_PI / 360.0f;
}

float rad2deg( float radians ) {
	return radians * 360.0f / D3DX_PI;
}

float distanceSq(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &target){
	return powf(origin.x-target.x,2) + powf(origin.y-target.y,2) + powf(origin.z-target.z,2);
}

float distance(const D3DXVECTOR3 &origin, const D3DXVECTOR3 &target){
	return sqrtf( distanceSq(origin, target) );
}

void normalizeVector3(D3DXVECTOR3 &dst, const D3DXVECTOR3 &src) {
	assert( D3DXVec3Length( &src ) > 0 );
	dst = src / sqrtf(src.x*src.x + src.y*src.y + src.z*src.z);
}

void fixMatrix(D3DXMATRIX &dst, const D3DXMATRIX &src) {
	D3DXVECTOR3 left(src(0,0), src(0,1), src(0,2));
	D3DXVECTOR3 up(src(1,0), src(1,1), src(1,2));
	D3DXVECTOR3 front(src(2,0), src(2,1), src(2,2));

	D3DXVec3Cross(&front, &left, &up);
	D3DXVec3Cross(&up, &front, &left);

	normalizeVector3(left, left);
	normalizeVector3(up, up);
	normalizeVector3(front, front);

	for(int i=0; i<4; ++i) {
		dst(0,i)=left[i];
		dst(1,i)=up[i];
		dst(2,i)=front[i];
		dst(3,i)=src(3,i);
	}
}

bool isFixedMatrix(const D3DXMATRIX &matrix) {
	D3DXVECTOR3 left(matrix(0,0), matrix(0,1), matrix(0,2));
	D3DXVECTOR3 up(matrix(1,0), matrix(1,1), matrix(1,2));
	D3DXVECTOR3 front(matrix(2,0), matrix(2,1), matrix(2,2));

	float normalLeft = sqrtf(left.x*left.x + left.y*left.y + left.z*left.z);
	float normalUp = sqrtf(up.x*up.x + up.y*up.y + up.z*up.z);
	float normalFront = sqrtf(front.x*front.x + front.y*front.y + front.z*front.z);

	if (normalLeft!=1 || normalUp!=1 || normalFront!=1)
		return false;

	float fl = D3DXVec3Dot(&front, &left);
	float fu = D3DXVec3Dot(&front, &up);
	float lu = D3DXVec3Dot(&left, &up);

	if( fl!=0 || fu!=0 || lu!=0 )
		return false;
	else
		return true;
}


void normalize(float &dst, float src, float minNorm, float maxNorm, float min, float max) {
	assert( maxNorm != minNorm );
	assert( max != min );
	dst = (((maxNorm-minNorm)/(max-min)) * (src-min)) + minNorm;

	//if(fabs(dst)<0.0001f)
	//	dst=0.0f;
}

void normalize(D3DXVECTOR2 &dst, const D3DXVECTOR2 &src, float minNorm, float maxNorm, float min, float max) {
	normalize(dst.x, src.x, minNorm, maxNorm, min, max);
	normalize(dst.y, src.y, minNorm, maxNorm, min, max);
}

// Status color must to be [0,1]
void formatColorVector4ToDword(DWORD &color, const D3DXVECTOR4 &statusColor) {
	color = 0x00000000;

	float alphaF;
	normalize(alphaF, statusColor.w, 0.0f, 255.0f, 0.0f, 1.0f);
	DWORD alpha = (int)alphaF;
	alpha = alpha << 24;
	color = color | alpha;

	float redF;
	normalize(redF, statusColor.x, 0.0f, 255.0f, 0.0f, 1.0f);
	DWORD red = (int)redF;
	red = red << 16;
	color = color | red;

	float greenF;
	normalize(greenF, statusColor.y, 0.0f, 255.0f, 0.0f, 1.0f);
	DWORD green = (int)greenF;
	green = green << 8;
	color = color | green;

	float blueF;
	normalize(blueF, statusColor.z, 0.0f, 255.0f, 0.0f, 1.0f);
	DWORD blue = (int)blueF;
	color = color | blue;
}

void formatColorDWordToVector4(D3DXVECTOR4 &statusColor, DWORD color) {
	// Alpha
	float alphaD = (float)(color & 0xFF000000);
	float alphaMin = (float)0x00FFFFFF;
	float alphaMax = (float)0xFFFFFFFF;
	normalize(statusColor.w, alphaD, 0.0f, 1.0f, alphaMin, alphaMax);

	// X
	float xD = (float)(color & 0x00FF0000);
	float xMin = (float)0x0000FFFF;
	float xMax = (float)0x00FFFFFF;
	normalize(statusColor.x, xD, 0.0f, 1.0f, xMin, xMax);

	// Y
	float yD = (float)(color & 0x0000FF00);
	float yMin = (float)0x000000FF;
	float yMax = (float)0x0000FFFF;
	normalize(statusColor.y, yD, 0.0f, 1.0f, yMin, yMax);

	// Z
	float zD = (float)(color & 0x000000FF);
	float zMin = (float)0x00000000;
	float zMax = (float)0x000000FF;
	normalize(statusColor.z, zD, 0.0f, 1.0f, zMin, zMax);
}

float obtainRandom(float min, float max) {
	float randomNorm = (float)rand();
	normalize(randomNorm, randomNorm, min, max, 0.0f, RAND_MAX);
	return randomNorm;
}

void rotateOnAxis( D3DXMATRIX &output, const D3DXMATRIX &input, const D3DXVECTOR3 &axis, float angle ) {
	// Checks
	if( angle == 0.0f ) {
		output = input;
		return;
	}

	D3DXVECTOR3 save_pos;
	save_pos.x = input(3,0);
	save_pos.y = input(3,1);
	save_pos.z = input(3,2);

	output = input;
	output(3,0) = output(3,1) = output(3,2) = 0.0f;

	D3DXMATRIX rot;
	D3DXMatrixRotationAxis( &rot, &axis, angle );
	D3DXMatrixMultiply( &output, &output, &rot );

	output(3,0) = save_pos.x;
	output(3,1) = save_pos.y;
	output(3,2) = save_pos.z;
}

bool obtainFrontPlayerUpScreen( D3DXVECTOR3 &out ) {
	const D3DXVECTOR3 &player_pos = Global::player->loc.getPosition( );
	const D3DXVECTOR3 &player_front = Global::player->loc.getLocalFront( );
	IPlayerController *player_controller = Global::player->getPlayerController( );

	if( player_controller != NULL ) {
		const char *player_controller_type = player_controller->getControllerType( );
		if( strcmp( player_controller_type, "SpherePlayerController" ) == 0 ) {
			float player_angle = ((SpherePlayerController *)player_controller)->getCurrentAngle( );

			D3DXMATRIX player_transform_no_rotated;
			rotateOnAxis( player_transform_no_rotated, Global::player->loc.getTransform( ), Global::player->loc.getLocalUp( ), -player_angle );

			out.x = player_transform_no_rotated(2,0);
			out.y = player_transform_no_rotated(2,1);
			out.z = player_transform_no_rotated(2,2);

			return true;
		}
	}

	out = D3DXVECTOR3( 0, 1, 0 );
	return false;
}

float clamp(float value, float min, float max) {
	if ( value < min ) 
		return min;
	else if( value > max )
		return max;
	else
		return value;
}

int clamp(int value, int min, int max) {
	if ( value < min ) 
		return min;
	else if( value > max )
		return max;
	else
		return value;
}

void lerp(D3DXVECTOR3 &dst, const D3DXVECTOR3 &src0, const D3DXVECTOR3 &src1, float t) {
	dst = t*src0 + (1.0f-t)*src1;
}

void lerp(D3DXQUATERNION &dst, const D3DXQUATERNION &src0, const D3DXQUATERNION &src1, float t) {
	float dot = D3DXQuaternionDot(&src0, &src1);
	if( dot > 0.0f )
		dst = t*src0 + (1.0f-t)*src1;
	else
		dst = t*src0 - (1.0f-t)*src1;
}

void rotateVector3Quaternion( D3DXVECTOR3 &dst, const D3DXVECTOR3 &vector3, const D3DXQUATERNION &quaternion )
{
	D3DXQUATERNION conjugated;
	D3DXQuaternionInverse(&conjugated, &quaternion);

	D3DXQUATERNION temp(conjugated * D3DXQUATERNION(vector3.x, vector3.y, vector3.z, 0.0f) * quaternion );

	dst.x = temp.x;
	dst.y = temp.y;
	dst.z = temp.z;
}
