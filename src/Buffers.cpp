#include <LucE/Buffers.hpp>

#include <string>
#include <glm/glm.hpp>
#include <glad.h>
#include <vector>
#include <iostream>

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

// render frame buffer
// -------------------
RenderFramebuffer::RenderFramebuffer(unsigned int width, unsigned int height)
{
    // create buffer
    glGenFramebuffers(1, &ID);
    glBindFramebuffer(GL_FRAMEBUFFER, ID);
    // bind color attachment
    glGenTextures(1, &colourBuffer);
    glBindTexture(GL_TEXTURE_2D, colourBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0);
    // create renderbuffer object for depth/stencil
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER::NOT_COMPLETE" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    setupPlane();
}

void RenderFramebuffer::drawQuad(Shader &shader)
{
    shader.use();
    shader.setInt("frameTexture", 0);
    glBindTexture(GL_TEXTURE_2D, colourBuffer);
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

// private
void RenderFramebuffer::setupPlane()
{
    const float quadVertices[]
    {   // position  // texcoord
        -1.0f, 1.0f, 0.0f, 1.0f, // top left
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        1.0f, -1.0f, 1.0f, 0.0f, // bottom right
        1.0f, 1.0f, 1.0f, 1.0f // top right
    };
    const unsigned int quadIndices[]
    {
        0, 1, 2, 2, 3, 0
    };

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices, GL_STATIC_DRAW);
    // attribute config
    // ----------------
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*) 0);
    // texCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*) (sizeof(float) * 2));

    glBindVertexArray(0);
}