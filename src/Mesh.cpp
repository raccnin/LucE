#include <LucE/Mesh.hpp>
#include <glm/glm.hpp>



Mesh::Mesh(Vertex vertices[], unsigned int vsize, unsigned int indices[], unsigned int isize, GLenum primType /* default GL_TRIANGLES */)
{
    // need indices length for drawing
    // -------------------------------
    this->elementCount = isize / sizeof(unsigned int);
    this->primType = primType;

    // bind and fill buffers
    // ---------------------
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, vsize, vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, indices, GL_STATIC_DRAW);

    // set vertex attributes
    // --------------------
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // unbind
    glBindVertexArray(0);
}
void Mesh::draw(Shader &shader)
{
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(primType, elementCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}