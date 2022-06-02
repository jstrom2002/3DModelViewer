#pragma once
#include "structs.hpp"

struct aiMaterial;
struct aiScene;

namespace TDModelView 
{
	class ASSIMPreader
	{
	public:
		std::string directory = "";
		std::string extension = "";
		std::string filepath = "";
		unsigned int flags = 0;
		aiScene* aiscene = nullptr;
		std::shared_ptr<Scene> scene = nullptr;
		ASSIMPreader(std::string filepath);
		~ASSIMPreader();
	private:
		std::unordered_multimap<int,std::shared_ptr<Mesh>> mesh_load_data;
		std::vector<Texture> embedded_textures;
		std::shared_ptr<Material> ImportMaterial(aiMaterial* mMaterial);
		void ImportMaterialTextures(aiMaterial* mMaterial, std::shared_ptr<Material> material);
		void ImportMaterials();
		void ImportMeshes();
		void ImportScene();
		void ImportTextures();
		void waitForMeshThreadsToFinish();
	};
}