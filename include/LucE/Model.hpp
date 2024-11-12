#ifndef LMODEL_H
#define LMODEL_H

#include <LucE/Shader.hpp>
#include <LucE/Mesh.hpp>

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
struct Light
{
    glm::vec3 position;

    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 specular;
};


struct Material
{
    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 specular;
    float shininess;
};

class Model
{
    public:
        glm::vec3 worldPos;
        std::vector<Texture> textures;

        Model(const std::string &path, const Material &material)
        {
            this->material = material;
            this->worldPos = glm::vec3(0.0f);
            this->scale = glm::vec3(1.0f);
            this->orientAngle = 0.0f;
            this->orientAxis = glm::vec3(0.0f, 1.0f, 0.0f);
            loadModel(path);
        }
        void draw(Shader &shader, Light &light);
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

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};

#endif