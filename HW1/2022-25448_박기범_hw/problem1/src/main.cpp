#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window,Shader shader);
//code got help from learnopengl.com
//use arrowkey to move and r to rotate
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint shaderProgram;


float pos_x = 0;
float pos_y = 0;
float rot = 0;
float cog_x = 0;
float cog_y = 0;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions
         0.5f, -0.25f, 0.0f,// 1.0f,0.0f,0.0f, // bottom right
        -0.5f, -0.25f, 0.0f,// 1.0f,0.0f,0.0f,// bottom left
         0.0f,  0.5f, 0.0f,//  1.0f,0.0f,0.0f// top
    };
    cog_x = (vertices[0] + vertices[3] + vertices[6]) / 3;
    cog_y = (vertices[1] + vertices[4] + vertices[7]) / 3;
    unsigned int indices[] = { 0,1,2 };
    unsigned int VBO, VAO;
    unsigned int VBO2, VAO2;

    /**********Fill in the blank*********/
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    /*************************************/


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, ourShader);

        // render
        // ------

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the triangle
        //ourShader.use();ver

        shaderProgram = ourShader.getID();
        glUseProgram(shaderProgram);
        float timeValue = glfwGetTime();
        float blue = int(fmod(timeValue,2));
        //vertices[0] += pos_x; vertices[3] += pos_x; vertices[6] += pos_x;
        //vertices[1] += pos_y; vertices[4] += pos_y; vertices[7] += pos_y;
        //cog_x += pos_x;
        //cog_y += pos_x;
        //pos_x = 0;
        //pos_y = 0;
        ourShader.use();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glm::mat4 transform = glm::mat4(1.0f);
        //transform = glm::translate(transform, glm::vec3(-1*pos_x, -1*pos_y, 0));
        transform = glm::translate(transform, glm::vec3(pos_x, pos_y, 0));
        transform = glm::rotate(transform, rot, glm::vec3(0.0f, 0.0f, 1.0f));

        //float blue = (fmod(timeValue,2)) + 0.5f;
        /**********Fill in the blank*********/
        
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "Frag_Color");
        glUniform4f(vertexColorLocation, 0.0f, 0.0f,blue, 1.0f);

        /*************************************/
        
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glBindVertexArray(VAO);



        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------











        glfwSwapBuffers(window);
        glfwPollEvents();
        glDeleteProgram(shaderProgram);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, Shader ourShader)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /**********Fill in the blank*********/
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pos_y+=0.002;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pos_y-=0.002;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        pos_x -= 0.002;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        pos_x += 0.002;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        rot += 0.005;


    /*************************************/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
