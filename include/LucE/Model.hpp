#ifndef LMODEL_H
#define LMODEL_H

#include <LucE/Shader.hpp>
#include <LucE/Mesh.hpp>

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int TextureFromFile(const char *path, const std::string &directory);

class Model
{
    public:
        Model(const std::string &path)
        {
            loadModel(path);
        }
        void draw(Shader &shader);
    private:
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};

#endif