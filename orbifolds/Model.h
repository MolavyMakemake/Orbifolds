#pragma once

#include "Shader.h"
#include <vector>
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb/stb_image.h>

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:
    Model(){}
    Model(const char* path)
    {
        loadModel(path);
    }
    Model(std::vector<Mesh>& meshes) {
        this->meshes = meshes;
    }
    void Draw(Shader& shader, Camera& camera);
    void Delete();

    void DrawGUI(const char* name);

    std::string getPath() {
        return path;
    }

    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 position;
    glm::quat rotation;

private:
    std::vector<Texture> textures_loaded;

    // model data
    std::vector<Mesh> meshes;
    std::string directory;
    std::string path;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);
};