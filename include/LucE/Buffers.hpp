#ifndef BUFFERS_H
#define BUFFERS_H

#include <string>
#include <glm/glm.hpp>

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
    MODEL,
    VIEW,
    PROJECTION
};

#endif