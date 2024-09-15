#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

#include "Camera.h"
#include "Light.h"


const std::string vertexPath = "Shader/";
const std::string fragmentPath = "Shader/";
const std::string shaderSuffix = ".glsl";

const std::string keyVPMatrix = "_VPMatrix";
class Shader
{
public:
    // 程序ID
    unsigned int ID;
    
    // 构造器读取并构建着色器
    Shader(std::string shaderName);
    // 使用激活程序
    void use();
    //场景数据相关
    void SetCameraProps(Camera camera);
    void SetLight(Light light);
    // uniform工具函数
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string& name, mat4x4 mat) const;
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
    void SearchIncludeFiles(std::string& shaderCode);
};

inline Shader::Shader(std::string shaderName)
{
    std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            std::string shaderVertexName = shaderName + "Vertex";
            std::string shaderFragmentName = shaderName + "Fragment";
            vShaderFile.open(vertexPath + shaderName + "/" + shaderVertexName + shaderSuffix);
            fShaderFile.open(fragmentPath + shaderName + "/" + shaderFragmentName + shaderSuffix);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
        
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        SearchIncludeFiles(vertexCode);
        SearchIncludeFiles(fragmentCode);
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
}
inline void Shader::use()
{
    glUseProgram(ID);
}

inline void Shader::SetCameraProps(Camera camera) 
{
    setMat4(keyVPMatrix, camera.GetVPMatrix());
}
inline void Shader::SetLight(Light light)
{
    glUniform3f(glGetUniformLocation(ID, "_LightPosition"), light.position.x, light.position.y, light.position.z);
    glUniform3f(glGetUniformLocation(ID, "_LightColor"), light.color.x, light.color.y, light.color.z);
    glUniform3f(glGetUniformLocation(ID, "_LightDirection"), light.direction.x, light.direction.y, light.direction.z);
}


inline void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
inline void Shader::setMat4(const std::string& name, mat4x4 mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


inline void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
inline void Shader::SearchIncludeFiles(std::string& shaderCode)
{
    std::regex regex(R"(#include\s+\"(.*)\")");
    std::smatch match;
    std::string::const_iterator start = shaderCode.cbegin();
    std::string::const_iterator end = shaderCode.cend();
    std::ifstream includeFile;
    std::stringstream shadersStream;
    while(std::regex_search(start, end, match, regex))
    {
        includeFile.open(match[1]);

        if(!includeFile.is_open())
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            return;
        }
        shadersStream.str("");
        shadersStream.clear();
        shadersStream << includeFile.rdbuf();
        shaderCode.replace(match.position(0), match.length(0), shadersStream.str());
        start = shaderCode.cbegin() + match.position(0) + shadersStream.str().length();
        end = shaderCode.cend();
        includeFile.close();
    }
}

#endif