#ifndef BUFFERS_H
#define BUFFERS_H

#include <string>
#include <glm/glm.hpp>
#include "Texture.hpp"
#include "Shader.hpp"

class UniformMat4Buf
{
    public:
        unsigned int ID;
        unsigned int bindIdx;
        const char* name;
        UniformMat4Buf(const std::string &name, const glm::mat4 data[], const unsigned int dataSize, const unsigned int bindIdx);
        void fillIdx(const unsigned int idx, const glm::mat4 &mat);
};

enum TransformIdx
{
    VIEW,
    PROJECTION
};


class Framebuffer
{
    public:
        unsigned int ID;
        Texture2D colourBuffer;

        Framebuffer(unsigned int width, unsigned int height, GLenum internal_format = GL_RGB);
        void use(){
            glBindFramebuffer(GL_FRAMEBUFFER, ID);
        }
};

class msFramebuffer
{
    public:
        unsigned int ID;
        unsigned int colourBuffer;

        msFramebuffer(unsigned int width, unsigned int height, unsigned int samples, GLenum internal_format = GL_RGB);
        void use(){
            glBindFramebuffer(GL_FRAMEBUFFER, ID);
        }
};

#endif
