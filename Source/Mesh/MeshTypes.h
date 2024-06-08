#ifndef MESH_TYPES_H
#define MESH_TYPES_H

#include "CoreMesh/CoreMesh.h"
#include "VertexTypes.h"

typedef CoreMesh<CUSTOMVERTEX_POSITION> CoreMeshPositioned;
typedef CoreMesh<CUSTOMVERTEX_TEXTURE> CoreMeshTextured;
typedef CoreMesh<CUSTOMVERTEX_NORMAL_TEXTURE> CoreMeshNormalTextured;
typedef CoreMesh<CUSTOMVERTEX_NORMAL_TAN_TEXTURE> CoreMeshNormalTanTextured;
typedef CoreMesh<CUSTOMVERTEX_COLOR> CoreMeshColored;
typedef CoreMesh<CUSTOMVERTEX_TEXTURE_LIGHTMAP> CoreMeshTexturedLightmap;
typedef CoreMesh<CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE> CoreMeshCal3DNormalTextured;

#endif //MESH_TYPES_H
