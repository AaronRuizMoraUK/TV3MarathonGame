#include <cassert>
#include "MeshFactory.h"
#include "Global/Rendering.h"
#include "Mesh/IMesh.h"
#include "Global/Variables.h"
#include "Mesh/MeshTypes.h"
#include "cal3d/cal3d.h"

struct Triangle;

template<class CUSTOMVERTEX>
void calculateTangents( IMesh *mesh );

void calculateTangentArray(int vertexCount, const D3DXVECTOR3 *vertex, const D3DXVECTOR3 *normal,
													 const D3DXVECTOR2 *texcoord, int triangleCount, const Triangle *triangle, 
													 D3DXVECTOR4 *tangent);

// ------------------------------------------------------------------------------

std::string MeshFactory::pathToMeshes = "";
MeshFactory * MeshFactory::meshFactory = NULL;

MeshFactory::~MeshFactory(void) {
	printDebugString("Destroying MeshFactory...\n");

	// Destroy mesh map
	MeshMap::const_iterator it = meshMap.begin();
	while(it !=  meshMap.end()) {	
		const IMesh *mesh = (it->second);
		if(mesh)
			delete mesh, mesh=NULL;
		++it;
	}
	meshMap.clear();
}

MeshFactory * MeshFactory::get() {
	if(meshFactory == NULL)
		meshFactory = new MeshFactory();

	return meshFactory;
}

// ------------------------------------------------------------------------------

const IMesh * MeshFactory::createMesh(const std::string &name, const CalCoreMesh *calCoreMesh) {
	MeshMap::const_iterator it = meshMap.find(name);

	if(it != meshMap.end())
		return it->second;
	else {
		const IMesh * mesh = convertCalCoreMeshToMesh(calCoreMesh);
		assert( mesh );
		meshMap[name]=mesh;
		return mesh;
	}
}

const IMesh * MeshFactory::createMesh(const std::string &filename) {
	MeshMap::const_iterator it = meshMap.find(filename);

	if(it != meshMap.end())
		return it->second;
	else {
		if( pathToMeshes.empty( ) )
			pathToMeshes = Global::data_locator.getPath("mesh");
		std::string pathFilename = pathToMeshes + filename;

		const IMesh * mesh = readMesh( pathFilename.c_str() );
		assert( mesh );
		meshMap[filename]=mesh;
		return mesh;
	}
}

// ------------------------------------------------------------------------------

const IMesh * MeshFactory::readMesh(const char *file) const {

	FILE *myFile = NULL;
	errno_t error = fopen_s( &myFile, file, "rb" );

	if( error != 0 || !myFile ) {
		warn( "Mesh file does not exists: %s", file );
		assert( !"Mesh file does not exists" );
		return NULL;
	}

	long sof = ftell( myFile );
	fseek( myFile, 0L, SEEK_END );
	long eof = ftell( myFile );
	size_t size = eof - sof;
	fseek( myFile, 0L, SEEK_SET );

	char *binary_file = new char[ size ];
	assert( binary_file );
	char *p = binary_file;

	// Read all file
	size_t size_readed = fread( binary_file, sizeof( char ), size, myFile );
	printfDebugString( "Reading mesh %s (%d/%d)\n", file, size_readed, size );
	assert( size_readed == size );

	// Close file
	fclose( myFile );

	// Get number of vertices
	int num_vertices;
	memcpy( &num_vertices, p, sizeof( int ) );
	p += sizeof( int );

	// Get Number of data to read of vertices (not used)
	int dataToRead;
	memcpy( &dataToRead, p, sizeof( int ) );
	p += sizeof( int );

	// Get number of faces
	int num_triangles;
	memcpy( &num_triangles, p, sizeof( int ) );
	p += sizeof( int );

	// Creamos la mesh
	IMesh *mesh = new CoreMeshNormalTanTextured(num_vertices, num_triangles);
	assert( mesh );

	// Vertices del objeto con coordenadas y textura
	CUSTOMVERTEX_NORMAL_TAN_TEXTURE * flatvertices = (CUSTOMVERTEX_NORMAL_TAN_TEXTURE *)mesh->getRawVertices();

	// Get Vertices
	for (int i=0; i<num_vertices; ++i) {
		flatvertices[i].x  = *((float *)p), p += sizeof( float );
		flatvertices[i].y  = *((float *)p), p += sizeof( float );
		flatvertices[i].z  = *((float *)p), p += sizeof( float );
		flatvertices[i].nx = *((float *)p), p += sizeof( float );
		flatvertices[i].ny = *((float *)p), p += sizeof( float );
		flatvertices[i].nz = *((float *)p), p += sizeof( float );
		flatvertices[i].u1 = *((float *)p), p += sizeof( float );
		flatvertices[i].v1 = *((float *)p), p += sizeof( float );
	}

	// Caras del objeto
	unsigned short * flatFaces = (unsigned short *)mesh->getRawIndexes();

	// Get Faces
	for (int i=0; i<num_triangles*3; ) {
		flatFaces[i++] = *(((int *)p)    ) - 1;
		flatFaces[i++] = *(((int *)p) + 1) - 1;
		flatFaces[i++] = *(((int *)p) + 2) - 1;

		p += sizeof( int )*3;
	}

	BoundingBox bbox;
	memcpy( &bbox.min_point, p, sizeof( float ) * 3 );
	p += sizeof( float ) * 3;

	memcpy( &bbox.max_point, p, sizeof( float ) * 3 );
	//p += sizeof( float ) * 3;

	delete [] binary_file;

	// CALCULATING TANGENTS
	calculateTangents<CUSTOMVERTEX_NORMAL_TAN_TEXTURE>( mesh );

	mesh->loadToVRAM();
	mesh->setBoundingBox( bbox );
	mesh->constructBoundingSphere();
	mesh->freeSystemMemory();

	return mesh;
}

// ------------------------------------------------------------------------------

const IMesh * MeshFactory::convertCalCoreMeshToMesh(const CalCoreMesh *calCoreMesh) const {
	const CalCoreSubmesh *calCoreSubmesh = NULL;

	if (calCoreMesh->getVectorCoreSubmesh().size() == 1)
		calCoreSubmesh = calCoreMesh->getCoreSubmesh(0);
	else
		calCoreSubmesh = calCoreMesh->getCoreSubmesh(1);

	if (calCoreSubmesh == NULL)
		return NULL;

	int skin_vertex_count = calCoreSubmesh->getVertexCount( );
	int skin_faces_count = calCoreSubmesh->getFaceCount();

	IMesh *mesh = new CoreMeshCal3DNormalTextured(skin_vertex_count, skin_faces_count);

	CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE * flatVertices = (CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE *)mesh->getRawVertices();

	const std::vector<CalCoreSubmesh::Vertex> &calVertexVector =  calCoreSubmesh->getVectorVertex( );
	const std::vector<std::vector<CalCoreSubmesh::TextureCoordinate> > &calTextureCoordinateVectors =  calCoreSubmesh->getVectorVectorTextureCoordinate();

	size_t sizeVector = calTextureCoordinateVectors.size();
	assert(sizeVector == 1);
	const std::vector<CalCoreSubmesh::TextureCoordinate> &calTextureCoordinateVector = calTextureCoordinateVectors[0];

	const CalCoreSubmesh::Vertex &calVertexFirst = calVertexVector[ 0 ];
	D3DXVECTOR3 minVertex(calVertexFirst.position.x, calVertexFirst.position.y, calVertexFirst.position.z);
	D3DXVECTOR3 maxVertex(minVertex);

	for (int i=0; i<skin_vertex_count; ++i) {
		const CalCoreSubmesh::Vertex &calVertex = calVertexVector[ i ];

		flatVertices[i].x = calVertex.position.x;
		flatVertices[i].y = calVertex.position.y;
		flatVertices[i].z = calVertex.position.z;

		if (flatVertices[i].x < minVertex.x)
			minVertex.x = flatVertices[i].x;

		if (flatVertices[i].y < minVertex.y)
			minVertex.y = flatVertices[i].y;

		if (flatVertices[i].z < minVertex.z)
			minVertex.z = flatVertices[i].z;

		if (flatVertices[i].x > maxVertex.x)
			maxVertex.x = flatVertices[i].x;

		if (flatVertices[i].y > maxVertex.y)
			maxVertex.y = flatVertices[i].y;

		if (flatVertices[i].z > maxVertex.z)
			maxVertex.z = flatVertices[i].z;

		flatVertices[i].nx = calVertex.normal.x;
		flatVertices[i].ny = calVertex.normal.y;
		flatVertices[i].nz = calVertex.normal.z;

		flatVertices[i].u1 = calTextureCoordinateVector[i].u;
		flatVertices[i].v1 = calTextureCoordinateVector[i].v;

		for( int j=0; j<4; ++j ) {
			flatVertices[i].weights[ j ] = 0.0f;
			flatVertices[i].indices[ j ] = 0.0f;
		}

		assert( calVertex.vectorInfluence.size( ) <= MAX_INFLUENCES );
		for( unsigned int j=0; j<calVertex.vectorInfluence.size( ); ++j ) {
			const CalCoreSubmesh::Influence &influence = calVertex.vectorInfluence[ j ];
			flatVertices[i].weights[ j ] = influence.weight;
			flatVertices[i].indices[ j ] = (float)influence.boneId;
		}

		//char msg[ 128 ];
		//sprintf_s(msg, sizeof(msg), "Vertice %d le influye el hueso %1.0f %1.0f %1.0f %1.0f con pesos %f %f %f %f\n", i, flatVertices[i].indices[0], flatVertices[i].indices[1], flatVertices[i].indices[2], flatVertices[i].indices[3], flatVertices[i].weights[0], flatVertices[i].weights[1], flatVertices[i].weights[2], flatVertices[i].weights[3]);
		//printDebugString(msg);

		// AARRGGBB -> weights[0] = BB ; weights[1] = GG ; weights[2] = RR ; weights[3] = AA 
		// So swat [0] <-> [2]
		/*float tmpVtx = flatVertices[i].weights[ 0 ];
		flatVertices[i].weights[ 0 ] = flatVertices[i].weights[ 2 ];
		flatVertices[i].weights[ 2 ] = tmpVtx;

		float tmpIdx = flatVertices[i].indices[ 0 ];
		flatVertices[i].indices[ 0 ] = flatVertices[i].indices[ 2 ];
		flatVertices[i].indices[ 2 ] = tmpIdx;*/
	}

	unsigned short * flatFaces = (unsigned short *)mesh->getRawIndexes();

	const std::vector<CalCoreSubmesh::Face> &calFaceVector =  calCoreSubmesh->getVectorFace( );

	for (int i=0; i<skin_faces_count; ++i) {
		const CalCoreSubmesh::Face &calFace = calFaceVector[i];

		// This way the indices because of the normals of the face
		// In cal3d are reversed
		flatFaces[i*3] = calFace.vertexId[2];
		flatFaces[i*3+1] = calFace.vertexId[1];
		flatFaces[i*3+2] = calFace.vertexId[0];
	}

	// CALCULATING TANGENTS
	calculateTangents<CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE>( mesh );

	mesh->loadToVRAM();
	mesh->setBoundingBox( BoundingBox( minVertex, maxVertex ) );
	mesh->constructBoundingSphere();
	mesh->freeSystemMemory();

	// This mesh comes from cal3d, so fix its Bounding Box and Shape
	mesh->fixBoundingBoxAndSphereForCal3D( );

	return mesh;
}

// ------------------------------------------------------------------------------
struct Triangle {
	unsigned short index[3];
};

template<class CUSTOMVERTEX>
void calculateTangents( IMesh *mesh ) {
	const int numVertices = mesh->getNumTotalVertices();
	const int numTriangles = mesh->getNumTotalTriangles();

	CUSTOMVERTEX *vertices = (CUSTOMVERTEX *) mesh->getRawVertices();
	unsigned short *indexes = (unsigned short *) mesh->getRawIndexes();

	D3DXVECTOR3 *positions = new D3DXVECTOR3[ numVertices ];
	D3DXVECTOR3 *normals   = new D3DXVECTOR3[ numVertices ];
	D3DXVECTOR2 *uvs       = new D3DXVECTOR2[ numVertices ];
	D3DXVECTOR4 *tangents  = new D3DXVECTOR4[ numVertices ];

	for( int i=0; i<numVertices; ++i ) {
		CUSTOMVERTEX &v = vertices[ i ];
		positions[ i ] = D3DXVECTOR3( v.x, v.y, v.z );
		normals[ i ] = D3DXVECTOR3( v.nx, v.ny, v.nz );
		uvs[ i ] = D3DXVECTOR2( v.u1, v.v1 );
	}

	calculateTangentArray( numVertices, positions, normals, 
		uvs, numTriangles, (const Triangle *) indexes, tangents );

	for( int i=0; i<numVertices; ++i ) {
		vertices[ i ].tx = tangents[ i ].x;
		vertices[ i ].ty = tangents[ i ].y;
		vertices[ i ].tz = tangents[ i ].z;
		vertices[ i ].tw = tangents[ i ].w;
	}

	delete [] positions;
	delete [] normals;
	delete [] uvs;
	delete [] tangents;
}

void calculateTangentArray(int vertexCount, const D3DXVECTOR3 *vertex, const D3DXVECTOR3 *normal,
													 const D3DXVECTOR2 *texcoord, int triangleCount, const Triangle *triangle, 
													 D3DXVECTOR4 *tangent)
{
	D3DXVECTOR3 *tan1 = new D3DXVECTOR3[vertexCount * 2];
	D3DXVECTOR3 *tan2 = tan1 + vertexCount;
	memset(tan1, 0, vertexCount * sizeof(D3DXVECTOR3) * 2);

	for (int a = 0; a < triangleCount; a++)
	{
		unsigned short i1 = triangle->index[0];
		unsigned short i2 = triangle->index[1];
		unsigned short i3 = triangle->index[2];

		const D3DXVECTOR3& v1 = vertex[i1];
		const D3DXVECTOR3& v2 = vertex[i2];
		const D3DXVECTOR3& v3 = vertex[i3];

		const D3DXVECTOR2& w1 = texcoord[i1];
		const D3DXVECTOR2& w2 = texcoord[i2];
		const D3DXVECTOR2& w3 = texcoord[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

        float temp = (s1 * t2 - s2 * t1);
		float r = 1.0f;
        if( fabsf(temp) >= 0.0001f )
            r = 1.0f / (s1 * t2 - s2 * t1);

		D3DXVECTOR3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		D3DXVECTOR3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] -= tdir;
		tan2[i2] -= tdir;
		tan2[i3] -= tdir;

		triangle++;
	}

	for (int a = 0; a < vertexCount; a++)
	{
		const D3DXVECTOR3& n = normal[a];
		const D3DXVECTOR3& t = tan1[a];

		// Gram-Schmidt orthogonalize
		D3DXVECTOR3 tm = t - n * D3DXVec3Dot(&n, &t);

		D3DXVec3Normalize( &tm, &tm );
		tangent[a].x = tm.x;
		tangent[a].y = tm.y;
		tangent[a].z = tm.z;

		// Calculate handedness
		D3DXVECTOR3 vcross;
		D3DXVec3Cross( &vcross, &n, &t );
		tangent[a].w = (D3DXVec3Dot( &vcross, &tan2[a]) < 0.0f) ? -1.0f : 1.0f;
	}

	delete[] tan1;
}
