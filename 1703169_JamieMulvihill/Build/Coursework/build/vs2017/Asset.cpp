#include "Asset.h"



Asset::Asset()
{
}


Asset::~Asset()
{
}

///functions to lod meshes
gef::Scene * Asset::LoadSceneAssets(gef::Platform & platform, const char * filename)
{
	gef::Scene* scene = new gef::Scene();

	if (scene->ReadSceneFromFile(platform, filename))
	{
		/// if scene file loads successful
		/// create material and mesh resources from the scene data
		scene->CreateMaterials(platform);
		scene->CreateMeshes(platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}
gef::Mesh * Asset::GetMeshFromSceneAssets(gef::Scene * scene)
{
	mesh = NULL;

	/// if the scene data contains at least one mesh
	/// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}