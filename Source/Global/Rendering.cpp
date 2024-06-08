#include "Directx/Directx.h"
#include "Mesh/VertexTypes.h"
#include "Variables.h"
#include "Camera/ICamera.h"
#include "Camera/Camera3D.h"
#include "Object/Location.h"
#include "Object/AnimatedObject/CalAnimModel.h"

void activeMatsOnDeviceForDrawsUP( ) {
#ifndef NDEBUG
	// Set World Matrix to the Device
	D3DXMATRIX identity;
	D3DXMatrixIdentity( &identity );
	Global::device->SetTransform( D3DTS_WORLD, &identity );

	getCamera3D( ).activeOnDevice( );
#endif
}

void drawLine (const D3DXVECTOR3 & src, const D3DXVECTOR3 & dst, unsigned int color )
{
#ifndef NDEBUG
	// Initialize vertices for axis
	CUSTOMVERTEX_COLOR g_points[] =
	{
		{ src.x, src.y, src.z, color },
		{ dst.x, dst.y, dst.z, color }
	};

	// Set the Vertex Declaration (FVF NOT USED)
	Global::device->SetVertexDeclaration( CUSTOMVERTEX_COLOR::getVertexDeclaration() );
	// Draw Lines with g_axis. User Primitive (UP)-> Stream is not needed
	Global::device->DrawPrimitiveUP(D3DPT_LINELIST, 1, g_points, sizeof(CUSTOMVERTEX_COLOR));
#endif
}

void drawDirection(float size) {
#ifndef NDEBUG
	drawLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,0.0f, 2.0f*size), 0xffffff00);
#endif
}

void drawAxis(float size) {
#ifndef NDEBUG
	// Draw axis indicating source vertice, destiny vertice an color
	drawLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f*size,0.0f, 0.0f),  0xffff0000);
	drawLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 2.0f*size, 0.0f), 0xff00ff00);
	drawLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,0.0f, 3.0f*size),  0xff0000ff);
#endif
}

void drawGridXZ () {
#ifndef NDEBUG
	float size = 0.5f;
	int nsamples = 50;
	float length = size * nsamples;

	for (int i=-nsamples; i<nsamples; ++i) {
		float j = i * size;
		drawLine(D3DXVECTOR3(j, 0.0f, -length), D3DXVECTOR3(j, 0.0f, length), 0xff3f3f3f);
		drawLine(D3DXVECTOR3(-length, 0.0f, j), D3DXVECTOR3(length, 0.0f, j), 0xff3f3f3f);
	}
#endif
}


void printDebugString( const char *msg ) {
#ifndef NDEBUG
	OutputDebugString( msg );
#endif
}

void printfDebugString( const char *msg, ... ) {
#ifndef NDEBUG
	char txt[512];
	va_list ap;

	va_start (ap, msg);
	_vsnprintf_s (txt, sizeof(txt), 512, msg, ap);
	va_end(ap);

	OutputDebugString( txt );
#endif
}

int print( int x, int y, int color, const char *msg ) {
#ifndef NDEBUG
	RECT rect = {x,y,0,0};
	Global::font->DrawText(NULL, msg, -1, &rect, DT_CALCRECT, 0);
	Global::font->DrawText(NULL, msg, -1, &rect, DT_LEFT, color );

	return (rect.bottom - rect.top);
#else
	return 0;
#endif
}

int printf( int x, int y, int color, const char *msg, ... ) {
#ifndef NDEBUG
	char txt[512];
	va_list ap;

	va_start (ap, msg);
	_vsnprintf_s (txt, sizeof(txt), 512, msg, ap);
	va_end(ap);

	return print( x, y, color, txt );
#else
	return 0;
#endif
}

void print( const LocationCollisionable &loc ) {
#ifndef NDEBUG
	if( loc.getCollisionObject() == NULL )
		return;

	D3DXMATRIX matrixCollision;
	DT_GetMatrixf( loc.getCollisionObject(), matrixCollision);
	char msg2[ 1024 ];
	memset( msg2, 0x00, sizeof( msg2 ) );
	strcat_s( msg2, sizeof( msg2 ), "Solid Position\n" );
	for( int i=0; i<4; ++i ) {
		for( int j=0; j<4; ++j ) {
			char msg_line[128];
			memset( msg_line, 0x00, sizeof( msg_line ) );
			sprintf_s(msg_line, sizeof(msg_line), "%f ", matrixCollision(i,j));
			strcat_s( msg2, sizeof( msg2 ), msg_line );
		}
		strcat_s( msg2, sizeof( msg2 ), "\n" );
	}
	char msg_line2[128];
	memset( msg_line2, 0x00, sizeof( msg_line2 ) );
	sprintf_s(msg_line2, sizeof(msg_line2), "\nObject Position\n(%f,%f,%f)\n"
		, loc.getPosition().x, loc.getPosition().y, loc.getPosition().z);
	strcat_s( msg2, sizeof( msg2 ), msg_line2 );
	printDebugString(msg2);
	MessageBox( NULL, msg2, "INF", MB_ICONERROR | MB_OK );
#endif
}

void print( const CalAnimModel *calAnimModel ) {
#ifndef NDEBUG
	if( calAnimModel == NULL )
		return;

	CalMixer *mixer = calAnimModel->calModel.getMixer();
	assert( mixer );
	int py=120;

	// Draw cyles
	typedef std::list<CalAnimationCycle *> LCycles;
	const LCycles & cycles = mixer->getAnimationCycle();
	LCycles::const_iterator itC = cycles.begin();
	while( itC != cycles.end() ){
		CalAnimationCycle *cycle = *itC;
		assert( cycle );
		assert( cycle->getCoreAnimation() );

		float current_time = mixer->getAnimationTime();
		float duration = cycle->getCoreAnimation()->getDuration();
		assert( duration != 0.0f );
		float percentage = current_time*100.0f/duration;
		const std::string &name = cycle->getCoreAnimation()->getFilename();
		py += printf(0, py, 0xffffffff, "Cycle %s: %0.3f seg -- %0.3f %%", name.c_str(), current_time, percentage);

		++itC;
	}

	py += 10;

	// Draw actions
	typedef std::list<CalAnimationAction *> LActions;
	const LActions & actions = mixer->getAnimationActionList();
	LActions::const_iterator itA = actions.begin();
	while (itA!=actions.end()){
		CalAnimationAction * action = *itA;
		assert( action );
		assert( action->getCoreAnimation() );

		float current_time = action->getTime();
		float duration = action->getCoreAnimation()->getDuration();
		assert( duration != 0.0f );
		float percentage = current_time*100.0f/duration;
		const std::string &name = action->getCoreAnimation()->getFilename();
		py += printf(0, py, 0xffffffff, "Action %s: %0.3f seg -- %0.3f %%", name.c_str(), current_time, percentage);

		++itA;
	}
#endif
}

void warn( const char *msg, ... ) {
#ifndef NDEBUG
	char txt[1024];
	va_list ap;

	va_start (ap, msg);
	_vsnprintf_s (txt, sizeof(txt), 512, msg, ap);
	va_end(ap);

	printDebugString( txt );
	MessageBox(NULL, txt, "Galaxy Scraper.exe", MB_ICONERROR | MB_OK);
#endif
}

void renderStateSetSolidFillMode() {
	Global::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void renderStateSetWireFrameFillMode() {
	Global::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

void renderStateEnableZTest() {
	Global::device->SetRenderState(D3DRS_ZENABLE, true);
	Global::device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

void renderStateDisableZTest() {
	Global::device->SetRenderState(D3DRS_ZENABLE, false);
}

void renderStateEnableZWriting() {
	Global::device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void renderStateDisableZWriting() {
	Global::device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void renderStateEnableAlphaTest() {
	Global::device->SetRenderState(D3DRS_ALPHATESTENABLE, true); 
	Global::device->SetRenderState(D3DRS_ALPHAREF, 0x00000005);
	Global::device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
}

void renderStateDisableAlphaTest() {
	Global::device->SetRenderState(D3DRS_ALPHATESTENABLE, false); 
}

void renderStateEnableAlphaBlending() {
	Global::device->SetRenderState( D3DRS_ALPHABLENDENABLE , true );
	Global::device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
	Global::device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
}

void renderStateEnableAdditiveAlphaBlending() {
	Global::device->SetRenderState( D3DRS_ALPHABLENDENABLE , true );
	Global::device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
	Global::device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_ONE);
}

void renderStateEnableSubtractiveAlphaBlending() {
	Global::device->SetRenderState( D3DRS_ALPHABLENDENABLE , true );
	Global::device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_ZERO);
	Global::device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
}

void renderStateDisableAlphaBlending() {
	Global::device->SetRenderState( D3DRS_ALPHABLENDENABLE , false );
}

void renderStateEnableCulling() {
	Global::device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
}

void renderStateDisableCulling() {
	Global::device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
}

void initRenderState( ) {
	// Disable Fixed Pipeline Lighting
	Global::device->SetRenderState( D3DRS_LIGHTING, false );

	// Disable Z Test
	renderStateDisableZTest( );

	// Disable Z Writing
	renderStateDisableZWriting( );

	// Disable Z Test
	renderStateDisableZTest( );

	// Disable Alpha Blending
	renderStateDisableAlphaBlending( );

	// Back-facing triangles culling in D3DCULL_CCW mode
	renderStateEnableCulling( );
}
