#ifndef UNIFORMMAT4BUF_H
#define UNIFORMMAT4BUF_H

#include <glm/glm.hpp>
#include <string>

enum TransformIdx
{
    VIEW,
    PROJECTION
};

class UniformMat4Buf
{
    public:
        unsigned int ID;
        unsigned int bindIdx;
        const char* name;
        UniformMat4Buf(const std::string &name, const glm::mat4 data[], const unsigned int dataSize, const unsigned int bindIdx);
        void fillIdx(const unsigned int idx, const glm::mat4 &mat);
};


#endif
