#ifndef MESH_H
#define MESH_H

#include <LucE/Shader.hpp>
#include <glm/glm.hpp>

using namespace std;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh
{
    public:
        unsigned int VAO;
        Mesh(const float* vertices, const unsigned int vsize, const unsigned int* indices, const unsigned int isize);
        void draw(Shader &shader);
    private:
        void constructMesh();
};
#endif