#ifndef SHADER_H
#define SHADER_H

#include<string>
#include<glad.h>
#include<glm/glm.hpp>

class Shader
{
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        void use();
        void setMat4(const std::string &name, const glm::mat4 &mat);
        void setVec3(const std::string &name, const glm::vec3 &vec);
        void setFloat(const std::string &name, float value);
        void setBlockBinding(const std::string &name, const unsigned int bindIndex);
        void setInt(const std::string &name, int value);
    private:
        unsigned int ID;
        unsigned int compileShader(const char* path, GLenum shaderType);
        void checkCompileError(GLuint shaderID, GLenum shaderType);
        void checkCompileError(GLuint shaderID);
};

#endif