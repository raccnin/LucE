#include <LucE/Shader.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <glad.h>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    unsigned int vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileError(ID, GL_PROGRAM);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


unsigned int Shader::compileShader(const char* path, GLenum shaderType)
{
    std::string shaderCode;
    std::ifstream shaderFile;
    // enable error bits
    shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // read file data into strings
        shaderFile.open(path);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    }
    catch(std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << e.what() << "\n";
    }
    const char* shaderCode_ptr = shaderCode.c_str();
    unsigned int shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderCode_ptr, NULL);
    glCompileShader(shaderID);    
    checkCompileError(shaderID, shaderType);
    return shaderID;
}


void Shader::checkCompileError(GLuint shaderID, GLenum shaderType)
{
    GLint success;
    GLchar infoLog[1024];
    std::string type;
    switch (shaderType)
    {
        case GL_VERTEX_SHADER:
            type = "VERTEX";
            break;
        case GL_FRAGMENT_SHADER:
            type = "FRAGMENT";
            break;
        default:
            break;
    }
    if (shaderType != GL_PROGRAM)
    {
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shaderID, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        } 
        else
        {
            std::cout << "compiled shader: " << type << " successfully\n";
        }
    }
    else
    {
        glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shaderID, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
        else
        {
            std::cout << "linked program: " << shaderID << "\n";
        }
    }
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setuMat4(const std::string &name, glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}