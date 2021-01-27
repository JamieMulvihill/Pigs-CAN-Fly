#pragma once
#include <graphics/mesh_instance.h>
#include "graphics/scene.h"
class Asset
{
public:
	Asset();
	~Asset();


	///functions to load scene assets into meshes 
	gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);

	void setMesh(gef::Mesh* mesh_) { asset_Mesh.set_mesh(mesh_); }

	gef::Mesh* mesh;
	gef::MeshInstance asset_Mesh;
};

