#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


// This is updated version of shader that can handle geometry and tessellation shader.
class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr
    , const char* tcsPath = nullptr, const char* tesPath = nullptr)
    {
        // shader Program
        ID = glCreateProgram();
        unsigned int vertex, fragment, geometry, tes, tcs;
        vertex = load_shader(vertexPath, GL_VERTEX_SHADER);
        fragment = load_shader(fragmentPath, GL_FRAGMENT_SHADER);
        if(geometryPath != nullptr)
            geometry = load_shader(geometryPath, GL_GEOMETRY_SHADER);
        if(tcsPath != nullptr){
            tcs = load_shader(tcsPath, GL_TESS_CONTROL_SHADER);
            tes = load_shader(tesPath, GL_TESS_EVALUATION_SHADER);
        }
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if(geometryPath != nullptr)
            glDeleteShader(geometry);
        if(tcsPath != nullptr){
            glDeleteShader(tcs);
            glDeleteShader(tes);
        }

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) 
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    unsigned int load_shader(std::string path, unsigned int shaderType){
        std::string code;
        std::ifstream file;
        file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            file.open(path);
            std::stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << file.rdbuf();	
            // close file handlers
            file.close();
            // convert stream into string
            code = shaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* shaderCode = code.c_str();
        unsigned int shaderID = glCreateShader(shaderType);
        int codeLength = code.length();
        glShaderSource(shaderID, 1, &shaderCode, &codeLength);
        glCompileShader(shaderID);
        std::string type;
        switch(shaderType){
            case GL_VERTEX_SHADER:
                type = "VERTEX";
                break;
            case GL_FRAGMENT_SHADER:
                type = "FRAGMENT";
                break;
            case GL_GEOMETRY_SHADER:
                type = "GEOMETRY";
                break;
            case GL_TESS_CONTROL_SHADER:
                type = "TCS";
                break;
            case GL_TESS_EVALUATION_SHADER:
                type = "TES";
                break;
            default:
                type = "NONE";
        }
        checkCompileErrors(shaderID, type);
        glAttachShader(ID, shaderID);
        return shaderID;
    }
};
#endif