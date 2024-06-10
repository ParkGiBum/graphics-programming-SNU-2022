#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
class VAO{
public:
    unsigned int ID;
    unsigned int associatedVBOID;
    unsigned int associatedEBOID;
    unsigned int vertexNumber;

    //std::vector<unsigned int> associatedVBOs;
};

VAO *getVAOFromAttribData(const std::vector<float> &attrib_data, const std::vector<unsigned int> &attrib_sizes, 
    const std::vector<unsigned int> &indices_data = std::vector<unsigned int>())
{
    VAO *vao = new VAO();
    //unsigned int VBO;
    glGenVertexArrays(1, &(vao->ID));
    glGenBuffers(1, &(vao->associatedVBOID));
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao->ID);
    glBindBuffer(GL_ARRAY_BUFFER, vao->associatedVBOID);
    glBufferData(GL_ARRAY_BUFFER, attrib_data.size() * sizeof(float), &attrib_data[0], GL_STATIC_DRAW);
    
    if (indices_data.size() > 0) {
        glGenBuffers(1, &(vao->associatedEBOID));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->associatedEBOID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_data.size() * sizeof(unsigned int), &indices_data[0], GL_STATIC_DRAW);
    }
    unsigned int offset = 0;
    unsigned int index = 0;
    unsigned int size_sum = 0;
    for(auto const& value: attrib_sizes){
        size_sum += value;
    }

    vao->vertexNumber = attrib_data.size() / size_sum;
    if (indices_data.size() > 0) {
        vao->vertexNumber = indices_data.size();
    }

    for(auto const& value: attrib_sizes){
        glVertexAttribPointer(index, value, GL_FLOAT, GL_FALSE, size_sum * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
        index++;
        offset += value;
    }
    return vao;
}
#endif
