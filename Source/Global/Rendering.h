#ifndef RENDERING_H
#define RENDERING_H

#include "Directx/Directx.h"

class LocationCollisionable;
struct CalAnimModel;

void activeMatsOnDeviceForDrawsUP( );

void drawLine (const D3DXVECTOR3 & src, const D3DXVECTOR3 & dst, unsigned int color );
void drawDirection(float size=1.0f);
void drawAxis(float size=1.0f);
void drawGridXZ();

void printDebugString( const char *msg );
void printfDebugString( const char *msg, ... );
int  print( int x, int y, int color, const char *msg );
int  printf( int x, int y, int color, const char *msg, ... );
void print( const LocationCollisionable &loc );
void print( const CalAnimModel *calAnimModel );
void warn( const char *msg, ... );

void renderStateSetSolidFillMode();
void renderStateSetWireframeFillMode();

void renderStateEnableZTest();
void renderStateDisableZTest();

void renderStateEnableZWriting();
void renderStateDisableZWriting();

void renderStateEnableAlphaTest();
void renderStateDisableAlphaTest();

void renderStateEnableAlphaBlending();
void renderStateEnableAdditiveAlphaBlending();
void renderStateEnableSubtractiveAlphaBlending();
void renderStateDisableAlphaBlending();

void renderStateEnableCulling();
void renderStateDisableCulling();

void initRenderState( );

#endif //RENDERING_H
