#include <LucE/UniformMat4Buf.hpp>
#include <glm/glm.hpp>
#include <glad.h>

UniformMat4Buf::UniformMat4Buf(const std::string &name, const glm::mat4 data[], const unsigned int dataSize, const unsigned int bindIdx)
{
    this->name = name.c_str();
    this->bindIdx = bindIdx;
    glGenBuffers(1, &ID);
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
    glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_STATIC_DRAW);
    for (unsigned int i = 0; i < dataSize; i++)
    {
        //ensure std130 format (may be unecessary step)
        glBufferSubData(GL_UNIFORM_BUFFER, i*sizeof(glm::mat4), sizeof(glm::mat4), &data[i][0][0]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindIdx, ID);
}   
void UniformMat4Buf::fillIdx(const unsigned int idx, const glm::mat4 &mat)
{
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
    glBufferSubData(GL_UNIFORM_BUFFER, idx*sizeof(glm::mat4), sizeof(glm::mat4), &mat[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


