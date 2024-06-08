#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H

#include "Directx/Directx.h"

/**
* Definition of vertices. FVF is not used, so it is
* necessary an element and a declaration.
*/

// POSITION and TEXTURE COORDINATE Vertex
extern const VertexElement CUSTOMVERTEX_POSITION_ELEMENT[];
extern VertexDeclaration CUSTOMVERTEX_POSITION_DECLARATION; // It must be created
struct CUSTOMVERTEX_POSITION
{
	float x, y, z;      // The local 3D position for the vertex

	// Return the Vertex Declaration (FVF NOT USED)
	static VertexDeclaration getVertexDeclaration( ) { return CUSTOMVERTEX_POSITION_DECLARATION; };
};

// POSITION and TEXTURE COORDINATE Vertex
extern const VertexElement CUSTOMVERTEX_TEXTURE_ELEMENT[];
extern VertexDeclaration CUSTOMVERTEX_TEXTURE_DECLARATION; // It must be created
struct CUSTOMVERTEX_TEXTURE
{
	float x, y, z;      // The local 3D position for the vertex
	float u1, v1;				// The texture coordenates for the vertex

	// Return the Vertex Declaration (FVF NOT USED)
	static VertexDeclaration getVertexDeclaration( ) { return CUSTOMVERTEX_TEXTURE_DECLARATION; };
};

// POSITION, NORMALS and TEXTURE COORDINATE Vertex
extern const VertexElement CUSTOMVERTEX_NORMAL_TEXTURE_ELEMENT[];
extern VertexDeclaration CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION; // It must be created
struct CUSTOMVERTEX_NORMAL_TEXTURE
{
	float x, y, z;      // The local 3D position for the vertex
	float nx, ny, nz;	// The normal direction for the vertex
	float u1, v1;		// The texture coordenates for the vertex

	// Return the Vertex Declaration (FVF NOT USED)
	static VertexDeclaration getVertexDeclaration( ) { return CUSTOMVERTEX_NORMAL_TEXTURE_DECLARATION; };
};

// POSITION, NORMALS, TANGENTS and TEXTURE COORDINATE Vertex
extern const VertexElement CUSTOMVERTEX_NORMAL_TAN_TEXTURE_ELEMENT[];
extern VertexDeclaration CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION; // It must be created
struct CUSTOMVERTEX_NORMAL_TAN_TEXTURE
{
	float x, y, z;      // The local 3D position for the vertex
	float nx, ny, nz;	// The normal direction for the vertex
	float tx, ty, tz, tw;	// The tangencial direction for the vertex
	float u1, v1;		// The texture coordenates for the vertex

	// Return the Vertex Declaration (FVF NOT USED)
	static VertexDeclaration getVertexDeclaration( ) { return CUSTOMVERTEX_NORMAL_TAN_TEXTURE_DECLARATION; };
};

// POSITION and COLOR Vertex
extern const VertexElement CUSTOMVERTEX_COLOR_ELEMENT[];
extern VertexDeclaration CUSTOMVERTEX_COLOR_DECLARATION; // It must be created
struct CUSTOMVERTEX_COLOR
{
	float x, y, z;      // The local 3D position for the vertex
	DWORD color;        // The vertex color

	// Return the Vertex Declaration (FVF NOT USED)
	static VertexDeclaration getVertexDeclaration( ) { return CUSTOMVERTEX_COLOR_DECLARATION; };
};

// POSITION and 2 TEXTURE COORDINATES Vertex
extern const VertexElement CUSTOMVERTEX_TEXTURE_LIGHTMAP_ELEMENT[];
extern VertexDeclaration CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION; // It must be created
struct CUSTOMVERTEX_TEXTURE_LIGHTMAP
{
	float x, y, z;      // The local 3D position for the vertex
	float u1, v1;		// The first texture coordenates for the vertex
	float u2, v2;		// The second texture coordenates for the vertex

	// Return the Vertex Declaration (FVF NOT USED)
	static VertexDeclaration getVertexDeclaration( ) { return CUSTOMVERTEX_TEXTURE_LIGHTMAP_DECLARATION; };
};

// POSITION, NORMALS, TANGENTS, TEXTURE COORDINATES, 4 BONE WEIGHTS and 4 BONE INDICES Vertex
extern const VertexElement CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_ELEMENT[];
extern VertexDeclaration CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION; // It must be created
struct CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE
{
	float x, y, z;				// The local 3D position for the vertex
	float nx, ny, nz;			// The normal direction for the vertex
	float tx, ty, tz, tw;			// The tangencial direction for the vertex
	float u1, v1;				// The texture coordenates for the vertex
	float weights[4];			// Weigth of each 4 bones that affect vertex
	float indices[4];			// Index of each 4 bones that affect vertex

	// Return the Vertex Declaration (FVF NOT USED)
	static VertexDeclaration getVertexDeclaration( ) { return CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE_DECLARATION; };
};

#endif //VERTEX_TYPES_H
