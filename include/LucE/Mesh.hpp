#ifndef MESH_H
#define MESH_H

#include <LucE/Shader.hpp>
#include <glm/glm.hpp>
#include <vector>

enum VERTEX_ATTRIB
{
    POSITION,
    NORMAL,
    TEXCOORD
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
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
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            setupMesh();
        }
        void draw(Shader &shader);

    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};

#endif