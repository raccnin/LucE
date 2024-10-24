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
        void setuMat4(const std::string &name, glm::mat4 &mat);
    private:
        unsigned int ID;
        unsigned int compileShader(const char* path, GLenum shaderType);
        void checkCompileError(GLuint shaderID, GLenum shaderType);
};

#endif