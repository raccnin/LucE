#ifndef SHADER_H
#define SHADER_H

#include<string>
#include<glad.h>

class Shader
{
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        void use();
    private:
        unsigned int ID;
        unsigned int compileShader(const char* path, GLenum shaderType);
        void checkCompileError(GLuint shaderID, GLenum shaderType);
};

#endif