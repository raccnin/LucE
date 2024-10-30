#ifndef MESH_H
#define MESH_H

#include <LucE/Shader.hpp>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
        void draw(Shader &shader);

    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};
#endif