#ifndef MESH_FACTORY_H
#define MESH_FACTORY_H

#include <map>
#include "Directx/Directx.h"

class IMesh;
class CalCoreMesh;

/**
* Factory of meshes.
*
* This is a singleton.
*/

class MeshFactory
{
protected:
	static std::string pathToMeshes;

	typedef std::map<std::string, const IMesh *> MeshMap;
	MeshMap meshMap;

public:
	static const int MAX_BONES = 40;
	static const int MAX_INFLUENCES = 3;

public:
	~MeshFactory();
	
	static MeshFactory * get();

	const IMesh * createMesh(const std::string &filename);
	const IMesh * createMesh(const std::string &name, const CalCoreMesh *calMesh);
	
private:
	static MeshFactory * meshFactory;
	MeshFactory() {};

	const IMesh * readMesh(const char *filename) const;
	const IMesh * convertCalCoreMeshToMesh(const CalCoreMesh *calMesh) const;
};

#endif //MESH_FACTORY_H
