#ifndef MESH_H
#define MESH_H

#include <LucE/Shader.hpp>
#include <glm/glm.hpp>
#include <vector>

using namespace std;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Texture
{
    unsigned int id;
    string type;
};

class Mesh
{
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        void draw(Shader &shader);

    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};
#endif