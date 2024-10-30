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
        Mesh(Vertex vertices[], unsigned int vsize, unsigned int indices[], unsigned int isize, GLenum primType = GL_TRIANGLES);
        void draw(Shader &shader);
    private:
        unsigned int elementCount;
        GLenum primType;
};
#endif