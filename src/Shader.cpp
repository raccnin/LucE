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
    checkCompileError(ID);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


unsigned int Shader::compileShader(const char* path, GLenum shaderType /* default nullptr */)
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
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n" << e.what() << "\n";
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
void Shader::checkCompileError(GLuint shaderID)
{
    GLint success;
    GLchar infoLog[1024];
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

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value)
{ 
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

void Shader::setFloat(const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setBlockBinding(const std::string &name, const unsigned int bindIndex)
{
    unsigned int block_idx = glGetUniformBlockIndex(ID, name.c_str());
    glUniformBlockBinding(ID, block_idx, bindIndex);
}