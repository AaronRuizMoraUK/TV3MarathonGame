#include <cassert>
#include "ParticleFactory.h"
#include "Global/Rendering.h"
#include "Mesh/IMesh.h"
#include "Mesh/MeshTypes.h"

ParticleFactory * ParticleFactory::particleFactory = NULL;

ParticleFactory::~ParticleFactory() {
	printDebugString("Destroying ParticleFactory...\n");

	// Destroy particle mesh map
	ParticleMeshMap::const_iterator it = particleMeshMap.begin();
	while(it !=  particleMeshMap.end()) {	
		const IMesh *mesh = (it->second);
		if(mesh)
			delete mesh, mesh=NULL;
		++it;
	}
	particleMeshMap.clear();
}

ParticleFactory * ParticleFactory::get() {
	if(particleFactory == NULL)
		particleFactory = new ParticleFactory();

	return particleFactory;
}

const IMesh * ParticleFactory::createParticle(int numParticles) {
	assert( numParticles > 0 );

	ParticleMeshMap::const_iterator it = particleMeshMap.find(numParticles);

	if(it != particleMeshMap.end())
		return it->second;
	else {
		const IMesh * mesh = createParticleMesh(numParticles);
		particleMeshMap[numParticles]=mesh;
		return mesh;
	}
}

const IMesh * ParticleFactory::createParticleMesh(int numParticles)
{
	float radiusSideX = 0.5f;
	float radiusSideY = 0.5f;
	unsigned int numQuadVertexPerAxis = 2;

	const float quadSizeX = radiusSideX*2.0f;
	const float quadSizeY = radiusSideY*2.0f;
	int numQuadVertices = numQuadVertexPerAxis * numQuadVertexPerAxis;
	int numQuadTriangles = ( numQuadVertexPerAxis - 1 ) * ( numQuadVertexPerAxis - 1 ) * 2;

	// Min and max vertex point
	D3DXVECTOR3 minPoint(0,0,0);
	D3DXVECTOR3 maxPoint(0,0,0);

	// Mesh
	IMesh *mesh = new CoreMeshPositioned(numQuadVertices * numParticles, numQuadTriangles * numParticles);
	assert(mesh);

	// Vertices
	CUSTOMVERTEX_POSITION *quadVertexData = (CUSTOMVERTEX_POSITION *)mesh->getRawVertices();	// Vertices del objeto con coordenadas y textura

	for (int particle = 0; particle < numParticles; ++particle)
	{
		for(unsigned int ix=0; ix<numQuadVertexPerAxis; ++ix) {

			float ux = (float) ix / (float) ( numQuadVertexPerAxis - 1 ); // ux = ix normalized

			CUSTOMVERTEX_POSITION *v = quadVertexData + ix * numQuadVertexPerAxis + particle * numQuadVertexPerAxis * numQuadVertexPerAxis; // Vertex to fill

			for(unsigned int iy=0; iy<numQuadVertexPerAxis; ++iy) {
				float uy = (float) iy / (float) ( numQuadVertexPerAxis - 1 ); // uy = iy normalized

				// Fill vertex data
				v[ iy ].x = quadSizeX * ( ux - 0.5f );
				v[ iy ].y = quadSizeY * ( uy - 0.5f );
				v[ iy ].z = particle * 2.0f; // Z is used to store the index -> * 2.0f para acceder a la informacion de particle_data en el shader
				// In Shader
				// u1 -> x+0.5
				// v1 -> 1.0f - (y+0.5) -> -y+0.5 -> 0.5-y
				//v[ iy ].u1 = ux;
				//v[ iy ].v1 = (1.0f-uy);

				// Update min and max point
				D3DXVECTOR3 vertex(v[iy].x, v[iy].y, v[iy].z);
				for(int i=0; i<3; ++i) {
					if(vertex[i]<minPoint[i])
						minPoint[i]=vertex[i];
					else if(vertex[i]>maxPoint[i])
						maxPoint[i]=vertex[i];
				}
			}
		}

		// Indexes
		unsigned short * quadIndexData = (unsigned short *)mesh->getRawIndexes();
		unsigned short *idx = quadIndexData + particle * numQuadVertexPerAxis * 3; // Pointer for covering quadIndexData to fill it

		for(unsigned int x=0; x<numQuadVertexPerAxis-1; ++x) {
			for(unsigned int y=0; y<numQuadVertexPerAxis-1; ++y) {
				unsigned short top = x * numQuadVertexPerAxis + y + (particle * numQuadVertexPerAxis * numQuadVertexPerAxis);
				*idx++ = top;
				*idx++ = top+1;
				*idx++ = top+numQuadVertexPerAxis;

				*idx++ = top+1;
				*idx++ = top+1+numQuadVertexPerAxis;
				*idx++ = top+numQuadVertexPerAxis;
			}
		}
	}

	mesh->loadToVRAM();
	mesh->setBoundingBox( BoundingBox( minPoint, maxPoint ) );
	mesh->constructBoundingSphere();
	mesh->freeSystemMemory();

	return mesh;
}
