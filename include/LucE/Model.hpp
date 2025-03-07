#ifndef LMODEL_H
#define LMODEL_H

#include <LucE/Shader.hpp>
#include <LucE/Mesh.hpp>

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct Material
{
	glm::vec3 albedo;
};

unsigned int TextureFromFile(const char* path, const std::string &directory);

class Model
{
    public:
        glm::vec3 worldPos;

        Model(std::string const &path)
        {
            this->worldPos = glm::vec3(0.0f);
            this->scale = glm::vec3(1.0f);
            this->orientAngle = 0.0f;
            this->orientAxis = glm::vec3(0.0f, 1.0f, 0.0f);
            loadModel(path);
        }
        void draw(Shader &shader);
        void setPos(glm::vec3 worldPos)
        {
            this->worldPos = worldPos;
        }
        void setPos(float x, float y, float z)
        {
            this->worldPos = glm::vec3(x, y ,z);
        }
        void uniformScale(float scale)
        {
            this->scale = glm::vec3(scale);
        }
        void setOrientation(float angle, glm::vec3 axis)
        {
            this->orientAngle = angle;
            this->orientAxis = axis;
        }
        void setOrientation(float angle)
        {
            this->orientAngle = angle;
        }
    private:
        Material material;
        glm::vec3 scale;
        float orientAngle;
        glm::vec3 orientAxis;
        std::vector<Mesh> meshes;
        std::string directory;
        glm::mat4 modelMat;
        std::vector<Texture> texturesLoaded;

        void loadModel(const std::string &path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string const &typeName);
};

#endif
