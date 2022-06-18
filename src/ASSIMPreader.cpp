#include "ASSIMPio.hpp"
#include "stdafx.h"
#include "structs.hpp"
#include <filesystem>
#include <assimp/IOSystem.hpp>
#include <assimp/pbrmaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/anim.h>
#include <assimp/light.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/pbrmaterial.h>
#include <assimp/scene.h>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>

namespace TDModelView 
{
    ASSIMPreader::~ASSIMPreader() {
        if (scene != nullptr) {
            scene->clear();
            scene.reset();
        }
    }
    std::shared_ptr<Material> ASSIMPreader::ImportMaterial(aiMaterial* mMaterial){
        std::shared_ptr<Material> material = std::make_shared<Material>();

        // assimp scene material name extract
        aiString matName;
        mMaterial->Get(AI_MATKEY_NAME, matName);
        float refracti, shininess, shinStrength, bumpMultiplier;

        // material factors
        refracti = 1.5f;
        mMaterial->Get(AI_MATKEY_REFRACTI, refracti);
        mMaterial->Get(AI_MATKEY_BUMPSCALING, bumpMultiplier);
        mMaterial->Get(AI_MATKEY_GLTF_ALPHACUTOFF, material->alphaCutoff);
        material->alphaCutoff = glm::clamp(material->alphaCutoff, 0.0f, 1.0f);

        //else
        {// Use ad hoc traditional method.
            mMaterial->Get(AI_MATKEY_SHININESS, shininess);
            mMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shinStrength);
            material->roughness = 1.0f - glm::clamp(shininess / 1000.0f, 0.0f, 1.0f);//Converts to range [0,1] for later conversion back to [0, inf]
            material->shininessStrength = (shinStrength);
            //if (shininess <= 1 || shininess > 9999) { shininess = 1.0f; }
            if (shinStrength < 0 || shinStrength > 9999) { shinStrength = 0; }
        }

        // For glTF files, use special pbr material keys from ASSIMP.
        //if (ext == ".gltf")
        //{
        mMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, material->diffuse);
        mMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, material->metalness);
        if (mMaterial->Get(AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS_GLOSSINESS_FACTOR, material->roughness) == AI_SUCCESS)
        {
            material->roughness = 1.0f - material->roughness;
        }
        if (mMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, material->roughness) == AI_SUCCESS)
        {
        }
        material->metalness = glm::clamp(material->metalness, 0.0f, 1.0f);
        //}

        if (bumpMultiplier <= -999999 || bumpMultiplier > 9999999)
            bumpMultiplier = 0.0;
        material->ior = refracti;
        material->bumpMultiplier = bumpMultiplier;

        // get material properties
        aiColor4D ambient(0.f), diffuse(1), specular(1, 1, 1, 0);
        aiColor4D emissive(0.f), transparent(0.f);
        mMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        mMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        mMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        mMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        if (mMaterial->Get(AI_MATKEY_OPACITY, material->opacity) == AI_SUCCESS)
        {
            material->opacity = glm::clamp(material->opacity, 0.0f, 1.0f);
        }
        else if (mMaterial->Get(AI_MATKEY_TRANSPARENCYFACTOR, material->opacity) == AI_SUCCESS)
        {
            material->opacity = glm::clamp(1.0f - material->opacity, 0.0f, 1.0f);
        }
        material->ambient = (glm::vec3(ambient.r, ambient.g, ambient.b));
        material->diffuse = (glm::vec3(diffuse.r, diffuse.g, diffuse.b));
        material->specular = (glm::vec3(specular.r, specular.g, specular.b));
        material->specularFactor = specular.a;
        material->emissive = (glm::vec3(emissive.r, emissive.g, emissive.b));
        mMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
        material->transparent = (glm::vec3(transparent.r, transparent.g, transparent.b));

        // Clamp values to reasonable range.
        material->roughness = glm::clamp(material->roughness, 0.0f, 1.0f);
        material->specularFactor = glm::clamp(material->specularFactor, 0.0f, 1.0f);
        material->ior = glm::clamp(material->ior, 1.0f, 20.0f);

        return material;
    }

    static glm::mat4 AIToGLMMat4(aiMatrix4x4 ai_mat) {
        glm::mat4 result;
        aiMatrix4x4 transposed = ai_mat.Transpose();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++)
            {
                result[i][j] = transposed[i][j];
            }
        }
        return result;
    }

    std::shared_ptr<Mesh> ImportMeshAsync(aiMesh* m, std::shared_ptr<Scene> scene,
        std::shared_ptr<Material> mat, std::string mesh_name, std::string scene_filepath){
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        mesh->material = mat;
        if (m->mNumVertices > 0){
            for (unsigned int i = 0; i < m->mNumVertices; i++){
                Vertex vertex;
                if (m->HasPositions())
                    vertex.position = glm::vec3(m->mVertices[i].x,m->mVertices[i].y,m->mVertices[i].z);                
                if (m->HasNormals())
                    vertex.normal = glm::vec3(m->mNormals[i].x,m->mNormals[i].y,m->mNormals[i].z);                
                for (int s = 0; s < m->GetNumUVChannels(); ++s){
                    if (m->HasTextureCoords(s))
                        vertex.uv = glm::vec3(m->mTextureCoords[s][i].x,m->mTextureCoords[s][i].y,m->mTextureCoords[s][i].z);                    
                }
                if (m->HasTangentsAndBitangents()){
                    vertex.tangent = glm::vec3(m->mTangents[i].x,m->mTangents[i].y,m->mTangents[i].z);
                    vertex.bitangent = glm::vec3(m->mBitangents[i].x,m->mBitangents[i].y,m->mBitangents[i].z);
                }
                //vertex.Orthonormalize();
                mesh->AddVertex(vertex);
            }
        }
        for (unsigned int i = 0; i < m->mNumFaces; i++){// Handle triangularized faces.
            if (m->mFaces[i].mNumIndices == 3) {
                mesh->AddIndex(m->mFaces[i].mIndices[0]);
                mesh->AddIndex(m->mFaces[i].mIndices[1]);
                mesh->AddIndex(m->mFaces[i].mIndices[2]);
            }
            else {// Handle polygonal faces.
                std::vector<unsigned int> idx;
                for (int k = 0; k < m->mFaces[i].mNumIndices; ++k)
                    idx.push_back(m->mFaces[i].mIndices[k]);                
                if (idx.size() > 3) {
                    for (int k = 0; k < idx.size() - 2; k++) {
                        mesh->AddIndex(idx[k + 0]);
                        mesh->AddIndex(idx[k + 1]);
                        mesh->AddIndex(idx[k + 2]);
                    }
                }
            }
        }
        if (!m->HasTangentsAndBitangents())
            mesh->calculateTangents();
        mesh->recalcBounds();
        return mesh;
    }
    void ASSIMPreader::ImportMeshes(){
        if (!aiscene->HasMeshes())
            return;

//#ifndef _DEBUG  // TO DO: add back in the openMP impl here, which causes errors and failures
//#pragma omp parallel for
//#endif
        for (int n = 0; n < aiscene->mNumMeshes; n++){
            std::string msh_name = aiscene->mMeshes[n]->mName.length > 0 ? std::string(aiscene->mMeshes[n]->mName.C_Str()) : "";
            mesh_load_data.emplace(n, ImportMeshAsync(aiscene->mMeshes[n],scene, scene->materials[aiscene->mMeshes[n]->mMaterialIndex],msh_name,this->filepath));
        }
    }
    void ASSIMPreader::ImportMaterials(){
        if (!aiscene->HasMaterials())
            return;
        for (unsigned int i = 0; i < aiscene->mNumMaterials; i++){
#ifdef _DEBUG
            checkError(std::string("Before loading textures for material: ") + std::string(aiscene->mMaterials[i]->GetName().C_Str()));
#endif
            auto newMaterial = ImportMaterial(aiscene->mMaterials[i]);
            ImportMaterialTextures(aiscene->mMaterials[i], newMaterial);
            scene->materials.push_back(newMaterial);
#ifdef _DEBUG
            checkError(std::string("After loading textures for material: ") + std::string(aiscene->mMaterials[i]->GetName().C_Str()));
#endif
        }
    }
    void ASSIMPreader::ImportTextures(){
        for (int i = 0; i < aiscene->mNumMaterials; ++i) {
            aiMaterial* material = aiscene->mMaterials[i];
            aiString texture_file;
            for (int j = 1; j<int(aiTextureType_UNKNOWN); ++j) {
                if (material->mNumProperties == 0 || material->Get(AI_MATKEY_TEXTURE(aiTextureType(j), 0), texture_file) == AI_FAILURE)                
                    continue;                

                // If loading a .gltf file, specific ASSIMP texture keys may be used for PBR textures.
                if ((extension == ".gltf" || extension == ".glb") &&
                    ((j == int(aiTextureType_DIFFUSE) || j == int(aiTextureType_BASE_COLOR)) &&
                        material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file) == AI_SUCCESS)

                    ||

                    ((j == int(aiTextureType_METALNESS) ||
                        j == int(aiTextureType_DIFFUSE_ROUGHNESS) ||
                        j == int(aiTextureType_AMBIENT_OCCLUSION)) &&
                        material->Get(AI_MATKEY_TEXTURE(aiTextureType_UNKNOWN, 0), texture_file) == AI_SUCCESS)
                    )
                {
                    // Material is loaded with texture_file path to proper PBR texture file.
                }
                else {
                    material->GetTexture(aiTextureType(j), 0, &texture_file);
                }

                if(texture_file.length > 0)
                    eng->textureBank->add(Texture(checkFilepath(std::string(texture_file.C_Str()), directory), directory));
            }
        }
    }
    void ASSIMPreader::ImportMaterialTextures(aiMaterial* mMaterial, std::shared_ptr<Material> material){

        bool usePBR = extension == ".gltf" || extension == ".glb";// this type of model will use PBR workflow textures.
        bool hMapToNormal = extension == ".obj"; // for whatever reason, ASSIMP registers obj normal maps as 'height maps'

        for (int i = 1; i <= int(aiTextureType_UNKNOWN); i++){
            aiTextureType texType = aiTextureType(i);
            unsigned int useChannel = 0;
            aiString texPath;
            // Check to see if this texture type is present in the material. If this isn't a valid file (ie getExtension finds no file extension), skip.
            if (!(mMaterial->GetTexture(aiTextureType(texType), 0, &texPath) == AI_SUCCESS))
                continue;


            // Interpret material texture data according to filetype.
            if (usePBR && ((texType == aiTextureType_DIFFUSE ||
                texType == aiTextureType_BASE_COLOR) && mMaterial->Get(AI_MATKEY_TEXTURE(
                    aiTextureType_DIFFUSE, 1), texPath) == AI_SUCCESS)) {
                texType = aiTextureType_BASE_COLOR;
            }
            else if (texType == aiTextureType_HEIGHT && hMapToNormal)
                texType = aiTextureType_NORMALS;

            // Check if this is a reference to an embedded texture. If so, find the necessary texture in the scene array. If not, load normally.
            if (std::string(texPath.C_Str()).rfind("*") != std::string::npos) {
                for (int n = 0; n < embedded_textures.size(); ++n) {
                    if (std::string(texPath.C_Str()) == embedded_textures[n].filepath) {
                        material->AddTexture(std::make_shared<Texture>(embedded_textures[n]), texType);
                        break;
                    }
                }
            }
            else {
                std::string dir = getDirectory(filepath);
                std::string fpath = checkFilepath(dir + std::string(texPath.data), directory);

                // Check this texture filepath to see if it's actually a file before loading.
                if (!std::filesystem::is_regular_file(fpath))
                    continue;

                if (!eng->textureBank->exists(fpath))                
                    eng->textureBank->add(Texture(fpath,directory));                                                   

                material->AddTexture(eng->textureBank->getPtr(fpath), texType);
                if (usePBR && texType == aiTextureType_UNKNOWN)
                {
                    if(!material->HasTexture(aiTextureType_DIFFUSE_ROUGHNESS))
                        material->AddTexture(eng->textureBank->getPtr(fpath), aiTextureType_DIFFUSE_ROUGHNESS);
                    if (!material->HasTexture(aiTextureType_AMBIENT_OCCLUSION))
                        material->AddTexture(eng->textureBank->getPtr(fpath), aiTextureType_AMBIENT_OCCLUSION);
                    if (!material->HasTexture(aiTextureType_METALNESS))
                        material->AddTexture(eng->textureBank->getPtr(fpath), aiTextureType_METALNESS);
                }
            }
        }
    }
    void ASSIMPreader::waitForMeshThreadsToFinish(){
        this->scene->meshes.resize(mesh_load_data.size());
        for (auto it = mesh_load_data.begin(); it != mesh_load_data.end(); ++it){
            this->scene->meshes[it->first] = it->second;
        }
    }
    void ASSIMPreader::ImportScene(){
        checkError(std::string("Before importing scene: ") + filepath);
        WriteToLogFile("Loading model " + this->filepath);
        scene = std::make_shared<Scene>();
        scene->materials.clear();

        // Do importing.
        ImportTextures();
        ImportMaterials();
        ImportMeshes();
        waitForMeshThreadsToFinish();

        // Do final sanity checks.
        if (scene->meshes.size() != aiscene->mNumMeshes)
            ErrorMessageBox("ERROR! Meshes not loaded properly.");
        if (scene->materials.size() != aiscene->mNumMaterials)
            ErrorMessageBox("ERROR! Materials not loaded properly.");

#ifdef _DEBUG
        checkError(std::string("After importing scene: ") + filepath);
#endif
        WriteToLogFile("Finished loading model file");
    }
    ASSIMPreader::ASSIMPreader(std::string filepath){
        eng->scene->clear();
        this->filepath = filepath;
        directory = getDirectory(filepath);
        extension = getExtension(filepath);
        Assimp::Importer importer;
        try {
            aiscene = (aiScene*)importer.ReadFile(filepath,
                aiProcess_CalcTangentSpace |
                aiProcess_JoinIdenticalVertices |
                aiProcess_Triangulate |
                aiProcess_GenUVCoords | 
                aiProcess_SortByPType |
                aiProcess_FixInfacingNormals |
                aiProcess_PreTransformVertices |
                aiProcess_TransformUVCoords |
                aiProcess_FindDegenerates | 
                aiProcess_GenNormals
                //aiProcess_GenSmoothNormals
            );
        }
        catch (std::exception e1) {
            ErrorMessageBox("ERROR! Could not load file. " + std::string(e1.what()));
        }
        if (!aiscene) {
            ErrorMessageBox("ERROR! Could not load file. " + std::string(importer.GetErrorString()));
            return;
        }
        ImportScene();
        try {
            importer.FreeScene();     
            eng->scene->copyToOutput(this->scene);
            eng->scene->m_Camera.position = eng->scene->bbox.center();
            eng->scene->m_Camera.position.z -= (eng->scene->bbox.extent().z * 2.5f);
            eng->scene->m_Camera.movementSpeed = glm::length(eng->scene->bbox.extent()) * 0.25f;
            eng->scene->m_Camera.Update();
            glfwSetWindowTitle(eng->window, getFilename(filepath).c_str());
        }
        catch (std::exception e1) {
            ErrorMessageBox("ERROR! Could not free aiScene. " + std::string(e1.what()));
        }
    }
}