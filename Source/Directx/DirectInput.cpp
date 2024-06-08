#include "DirectInput.h"

HRESULT createDeviceInputCreator(DeviceInputCreator &deviceInputCreator) {
	HRESULT hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION
		, IID_IDirectInput8, (VOID**)&deviceInputCreator, NULL );

	return FAILED( hr ) ? hr : S_OK;
}
