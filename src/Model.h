#pragma once

#include <assimp/Importer.hpp>
#include <utility>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Material.h"

const string modelPath = "resources/objects/nanosuit/nanosuit.obj";
class Model
{
public:
    Model(string modelName, string shaderName = "Lit");
    ~Model(){}

    void PreRender();
    void Render();
private:
    vector<Material> materials;
    vector<Mesh> meshes;
    string shaderName;
    
    void LoadModel(const char* path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
};

inline Model::Model(string modelName, string shaderName)
{
    string path = modelPath + modelName;
    LoadModel(path.c_str());
    this->shaderName = std::move(shaderName);
}

inline void Model::LoadModel(const char* path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    ProcessNode(scene->mRootNode, scene);
}

inline void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {

        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

inline void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    //顶点
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vert;

        vec2 vector2;
        vec3 vector3;
        vec4 vector4;
        //位置
        vector3.x = mesh->mVertices[i].x;
        vector3.y = mesh->mVertices[i].y;
        vector3.z = mesh->mVertices[i].z;
        vert.position = vector3;

        //法线
        vector3.x = mesh->mNormals[i].x;
        vector3.y = mesh->mNormals[i].y;
        vector3.z = mesh->mNormals[i].z;
        vert.normal = vector3;

        if(mesh->HasVertexColors(0))
        {
            vector4.x = mesh->mColors[0][i].r;
            vector4.y = mesh->mColors[0][i].g;
            vector4.z = mesh->mColors[0][i].b;
            vector4.w = mesh->mColors[0][i].a;
            vert.color = vector4;
        }

        //uv
        for(unsigned int j = 0; j < 3; j++)
        {
            if(mesh->mTextureCoords[j])
            {
                vector2.x = mesh->mTextureCoords[j][i].x;
                vector2.y = mesh->mTextureCoords[j][i].y;
                vert.uv[j] = vector2;
                if(j == 0)
                {
                    vector3.x = mesh->mTangents[i].x;
                    vector3.y = mesh->mTangents[i].y;
                    vector3.z = mesh->mTangents[i].z;
                    vert.tangent = vector3;
                }
            }
            else
            {
                vert.uv[j] = vec2(0.0f);
            }
        }
        vertices.push_back(vert);
    }
    //索引
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    //图片信息
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    
    materials.emplace_back(textures, shaderName);
    meshes.emplace_back(vertices, indices);
    
}

inline vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        string fileName = str.C_Str();
        Texture texture(texturePath + fileName);
        textures.push_back(texture);
       
    }
    return textures;
}





