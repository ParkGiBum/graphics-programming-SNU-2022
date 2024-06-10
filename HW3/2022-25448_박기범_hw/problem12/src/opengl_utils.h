#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
using namespace std;
class VAO{
public:
    unsigned int ID;
    unsigned int associatedVBOID;
    unsigned int dataSize;
};
// This function is very general VAO loader. Make use of it.
VAO *getVAOFromAttribData(const std::vector<float> &attrib_data, const std::vector<unsigned int> &attrib_sizes)
{
    VAO *vao = new VAO();
    //unsigned int VBO;
    glGenVertexArrays(1, &(vao->ID));
    //cout << "test" << endl;
    //cout << &(vao->ID) << endl;
    //cout << "test" << endl;
    glGenBuffers(1, &(vao->associatedVBOID));
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao->ID);
    glBindBuffer(GL_ARRAY_BUFFER, vao->associatedVBOID);
    glBufferData(GL_ARRAY_BUFFER, attrib_data.size() * sizeof(float), &attrib_data[0], GL_STATIC_DRAW);

    unsigned int offset = 0;
    unsigned int index = 0;
    unsigned int size_sum = 0;
    for(auto const& value: attrib_sizes){
        size_sum += value;
    }

    vao -> dataSize = attrib_data.size() / size_sum;

    for(auto const& value: attrib_sizes){
        glVertexAttribPointer(index, value, GL_FLOAT, GL_FALSE, size_sum * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
        index++;
        offset += value;
    }
    return vao;
}
#endif
