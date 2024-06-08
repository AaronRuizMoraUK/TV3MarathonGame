#include "Directx.h"

DeviceCreator createDeviceCreator() {
	return Direct3DCreate9( D3D_SDK_VERSION );
}
