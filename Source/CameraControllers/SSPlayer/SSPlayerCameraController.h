#ifndef SS_PLAYER_CAMERA_CONTROLLER_H
#define SS_PLAYER_CAMERA_CONTROLLER_H

#include "Directx/Directx.h"
#include "CameraControllers/ICameraController.h"

/**
* Camera Controller for a spherical scene
*/

class SSPlayerCameraController : public ICameraController {
public:
	enum VibrationIntensity {
		LOW = 0
		, MEDIUM
		, HIGH
	};

protected:
	// Speeds for the camera controller 
	float targetSpeed;
	float positionSpeed;

	// Distance between camera and player
	float distance_player;

	static const float low_vib_amount;
	static const float low_vib_speed;
	static const float medium_vib_amount;
	static const float medium_vib_speed;
	static const float high_vib_amount;
	static const float high_vib_speed;
	VibrationIntensity vib_intensity;
	float max_vib_time;
	float vib_time;
	bool vib_enabled;

public:
	SSPlayerCameraController(float pTargetSpeed, float pPositionSpeed, float pDistance);
	~SSPlayerCameraController();

	void update(Camera3D &camera, float deltaTime);

	void vibrateDuring( float seconds, VibrationIntensity avib_intensity = MEDIUM );

private:
	void setUpVector( Camera3D &camera, float deltaTime );
	void vibrateCamera( D3DXVECTOR3 &new_target, const Camera3D &camera, float deltaTime );
};

#endif //SS_PLAYER_CAMERA_CONTROLLER_H
