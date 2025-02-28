#include <LucE/Mesh.hpp>
#include <glm/glm.hpp>

#include <string>

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // attribute config
    // ----------------
    // position
    glEnableVertexAttribArray(vPOSITION);
    glVertexAttribPointer(vPOSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    // normal
    glEnableVertexAttribArray(vNORMAL);
    glVertexAttribPointer(vNORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    // tex coord
    glEnableVertexAttribArray(vTEXCOORD);
    glVertexAttribPointer(vTEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoord));
    // 
    glEnableVertexAttribArray(vTANGENT);
    glVertexAttribPointer(vTANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));

    glEnableVertexAttribArray(vBITANGENT);
    glVertexAttribPointer(vBITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));

    glBindVertexArray(0);
}

void Mesh::draw(Shader &shader)
{
    // bind material textures accordingly
    // format of: texture_{type}N
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 1; i < textures.size() + 1; i++) // texture unit 0 reserved for shadow map
    {
        // 1. bind next sampler
        glActiveTexture(GL_TEXTURE0+i);
        std::string number;
        std::string name = textures[i].type;
        // 2. determine texture name, then add to material struct
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        // set appropriate texture sampler to name convention
        shader.setInt(("material."+ name + number).c_str(), i);
        // 3. bind appropriate texture to previously bound sampler
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    // restore texture state to sampler 0
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // restore global states
    glBindVertexArray(0);
}
