#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))  
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))  
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "gameobjects.h"
#include "geometry_primitives.h"
#include <iostream>
#include <vector>
#include "text_renderer.h"
#include <cstdlib> 
//code got help from learnopengl.com
//generate triangle every 2 second
//score increase when it reaches bottom
//it will print score when triangle touchs body
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Bar& bar, float dt);
bool collision(float* vertex1, float* vertex2);
// setting
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 600;
GLuint shaderProgram;
bool previousKeyState[1024];
float pos_x = 0;
float pos_y = 0;
float rot = 0;
float cog_x = 0;
float cog_y = 0;
bool every_second = true;
int score = 0;
struct triangle
{
    unsigned int VBO, VAO;
    float speed;
    float rot_speed;
    int exist = 0;
    float pos_x;
    float pos_y;
    float dist = 0;

};

triangle traingle_array[1000];
triangle traingle_array2[20];
int num_of_triangle = 0;


double getRandomNumber(int min, int max) {
    double ans = (min + (rand() * (int)(max - min) / RAND_MAX)) ;
    return ans/1000;
}
int next_finder(triangle* traingle_array)
{
    int cnt = 0;
    for (int i = 0; i < 1000; i++)
    {
        if (traingle_array[i].exist == 0)
        {
            traingle_array[i].exist = 1;
            
            return(i);
        }
    }
    return 1000;
}

void draw_triangle(triangle* traingle_array, Shader ourShader2, Shader ourShader3, GLFWwindow* window)
{
    for (int k = 0; k < score % 20; k++)
    {
        ourShader2.use();
        glm::mat4 transform = glm::mat4(1.0f);
        unsigned int transformLoc = glGetUniformLocation(ourShader2.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        glBindVertexArray(traingle_array2[k].VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    for (int i = 0; i < 1000; i++) {
        if (traingle_array[i].exist)
        {
            float timeValue = glfwGetTime();
            float pos_xx = traingle_array[i].pos_x;
            float pos_yy = traingle_array[i].pos_y;
            if (pos_yy - 1 * (timeValue - traingle_array[i].dist) * traingle_array[i].speed <-1.0f)
            {
                traingle_array[i].pos_y = 100.0f;
                traingle_array[i].exist = 0;
                traingle_array[i].dist = 0;
                score+=1;
            }
            else
            {
                


                //std::cout << score << std::endl;
                ourShader2.use();

                glm::mat4 transform = glm::mat4(1.0f);
                
                //traingle_array[i].dist -= traingle_array[i].speed * 0.001;
                
                transform = glm::translate(transform, glm::vec3(pos_xx, pos_yy - 1 * (timeValue-traingle_array[i].dist)*traingle_array[i].speed, 0));
                transform = glm::rotate(transform, traingle_array[i].rot_speed * timeValue * 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                transform = glm::translate(transform, glm::vec3(-1 * pos_xx, -1 * pos_yy, 0));
                //
                unsigned int transformLoc = glGetUniformLocation(ourShader2.ID, "transform");
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
                glBindVertexArray(traingle_array[i].VAO);
                //unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
                //glm::mat4 transform = glm::mat4(1.0f);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                //transform = glm::translate(transform, glm::vec3(0.5f, 0.0f, 0));

                if (-0.8f < pos_yy - 1 * (timeValue - traingle_array[i].dist) * traingle_array[i].speed && pos_yy - 1 * (timeValue - traingle_array[i].dist) * traingle_array[i].speed  < -0.75f)
                {
                    //std::cout << "near_collision" << std::endl;
                    if (pos_x -0.15f <pos_xx && pos_xx < pos_x + 0.15f)
                    {
                        
                        glfwSetWindowShouldClose(window, true);
                        std::cout <<"Your score is " << score << std::endl;
                    }
                }

            }
                
        }
    }

}
void triangle_gen(triangle* traingle_array,  float timeValue)
{
    //std::cout << every_second << std::endl;
    if ((int(timeValue)*3 % 2) == 1 && every_second)
    {
        //generator
        

        every_second = false;
        std::srand(99999* timeValue);
        float scale = getRandomNumber(0, 1000); //0.5~1
        float loc = getRandomNumber(0, 1000); //0~1
        std::cout << scale << std::endl;
        std::cout << loc << std::endl;
        loc = 0.85 * (loc * 2 - 1);
        float vertices_tmp[] = {
           
             0.15f* scale + loc, -0.1f * scale+0.8f, 0.0f, 1.0f,0.0f,0.0f,// 1.0f,0.0f,0.0f, // bottom right
            -0.15f * scale + loc, -0.1f * scale + 0.8f, 0.0f,0.0f,1.0f,0.0f,// 1.0f,0.0f,0.0f,// bottom left
             0.0f * scale + loc,  0.2f * scale + 0.8f, 0.0f,0.0f,0.0f,1.0f//  1.0f,0.0f,0.0f// top
        };
        cog_x = (vertices_tmp[0] + vertices_tmp[3] + vertices_tmp[6]) / 3;
        cog_y = (vertices_tmp[1] + vertices_tmp[4] + vertices_tmp[7]) / 3;

        traingle_array[num_of_triangle].dist = timeValue;
        //num_of_triangle += 1;
        glGenVertexArrays(1, &(traingle_array[num_of_triangle].VAO));
        glGenBuffers(1, &(traingle_array[num_of_triangle].VBO));


        glBindVertexArray(traingle_array[num_of_triangle].VAO);

        glBindBuffer(GL_ARRAY_BUFFER, traingle_array[num_of_triangle].VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_tmp), vertices_tmp, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        traingle_array[num_of_triangle].speed = getRandomNumber(0, 1000);
        traingle_array[num_of_triangle].rot_speed = getRandomNumber(0, 1000);
        traingle_array[num_of_triangle].exist = 1;
        traingle_array[num_of_triangle].pos_x = loc;
        traingle_array[num_of_triangle].pos_y = 0.8f;
        num_of_triangle = next_finder(traingle_array);


    }
    if ((int(timeValue)*3 % 2) == 0)
    {
        //std::cout << 1272 << std::endl;
        every_second = true;
    }

    float vertices_tmp2[] = {

         -1.0f + 0.12f + 0.12f * (float)(score % 20), -0.9f, 0.0f, 1.0f,0.0f,0.0f,// 1.0f,0.0f,0.0f, // bottom right
        -1.0f + -0.00f  + 0.12f * (float)(score % 20), -0.9f , 0.0f,0.0f,1.0f,0.0f,// 1.0f,0.0f,0.0f,// bottom left
         -1.0f + 0.06f  + 0.12f * (float)(score % 20),  -0.8f , 0.0f,0.0f,0.0f,1.0f//  1.0f,0.0f,0.0f// top
    };
    traingle_array2[num_of_triangle].dist = timeValue;
    //num_of_triangle += 1;
    glGenVertexArrays(1, &(traingle_array2[score % 20].VAO));
    glGenBuffers(1, &(traingle_array2[score % 20].VBO));


    glBindVertexArray(traingle_array2[score % 20].VAO);

    glBindBuffer(GL_ARRAY_BUFFER, traingle_array2[score % 20].VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_tmp2), vertices_tmp2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    traingle_array2[score % 20].speed = getRandomNumber(0, 1000);
    traingle_array2[score % 20].rot_speed = getRandomNumber(0, 1000);
    traingle_array2[score % 20].exist = 1;

}

int main()
{
    
    // glfw: initialize and configure
    // ------------------------------'
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
    Shader ourShader2("../shaders/shader2.vs", "../shaders/shader2.fs"); // you can name your shader files however you like
    Shader ourShader3("../shaders/shader3.vs", "../shaders/shader3.fs");

    /////////////////////////////////////////////////////
    // TODO : Define VAO and VBO for triangle and quad(bar).
    /////////////////////////////////////////////////////
    unsigned int VBO_bar, VAO_bar, EBO_bar;
    float vertices[] = {
     0.15f, -0.75f, 0.0f,  // top right
     0.15f, -0.8f, 0.0f,  // bottom right
    -0.15f, -0.8f, 0.0f,  // bottom left
    -0.15f, -0.75f, 0.0f   // top left 
    };
    unsigned int indices[] = { 
        0, 1, 3,  
        1, 2, 3  
    };
    glGenVertexArrays(1, &VAO_bar);
    glGenBuffers(1, &VBO_bar);
    glGenBuffers(1, &EBO_bar);
    glBindVertexArray(VAO_bar);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_bar);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_bar);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::vector<Entity> entities;

    
    TextRenderer  *Text;
    Text = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
    Text->Load("../fonts/OCRAEXT.TTF", 24);

    Bar bar{ 0,0,0 };

    // render loop
    // -----------
    float generationInterval = 0.3f;
    float dt = 0.0f;
    //int score = 0;

    float barWidth = 0.3f;
    float barHeight = 0.05f;
    float barYPosition = -0.8f;
    
    
   
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    while (!glfwWindowShouldClose(window))
    {
        float timeValue = glfwGetTime();
        triangle_gen(traingle_array,  timeValue);
        // input
        // -----
        processInput(window, bar, dt);
        //std::cout << getRandomNumber(0, 1000)<<std::endl;
        // clear background
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram = ourShader.ID;
        glUseProgram(shaderProgram);

        glm::mat4 transform = glm::mat4(1.0f);
        //std::cout << pos_x << std::endl;
        //vertices[1] += pos_y; vertices[4] += pos_y; vertices[7] += pos_y; vertices[10] += pos_y;
        
            
            //vertices[0] += pos_x; vertices[3] += pos_x; vertices[6] += pos_x; vertices[9] += pos_x;
        transform = glm::translate(transform, glm::vec3(pos_x, 0.0f, 0));
        
        
            //cog_y += pos_y;
        //pos_x = 0;
        //pos_y = 0;
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindBuffer(GL_ARRAY_BUFFER, VBO_bar);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "Frag_Color");
        glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
        /////////////////////////////////////////////////////
        // TODO : Main Game Loop
        /////////////////////////////////////////////////////

        // --------------------
        
        //draw triangle///////////////////////////////////////
        draw_triangle(traingle_array, ourShader2,ourShader3,window);

        //////////////////////////////////////////////////////

        // (1) generate new triangle at the top of the screen for each time interval
        // (2) make triangles fall and rotate!!!!!
        // (3) Every triangle has different shape, falling speed, rotating speed.
        // (4) Render a red box
        // (5) Implement simple collision detection. Details are on the pdf file.
        // (6) Visualize the score & print score (you can use TextRenderer)
        Text->RenderText("SCORE : " + std::to_string(score), 5.0f, 5.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        //std::cout << std::to_string(score) << std::endl;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        ourShader.use();
        //glm::mat4 transform = glm::mat4(1.0f);
        glBindVertexArray(VAO_bar);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    glDeleteVertexArrays(1, &VAO_bar);
    glDeleteBuffers(1, &VBO_bar);
    glDeleteBuffers(1, &EBO_bar);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Bar& bar, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /**********Fill in the blank*********/
    //if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    //    pos_y += 0.002;
    //if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    //    pos_y -= 0.002;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        pos_x = MAX(pos_x-0.0005,-0.85f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        pos_x = MIN(pos_x + 0.0005,0.85f);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        rot = 0.5;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



bool collision(float* vertex1, float* vertex2)
{
    float cog_x = vertex2[0] + vertex2[3] + vertex2[6];
    cog_x = cog_x / 3;
    float cog_y = vertex2[1] + vertex2[4] + vertex2[7];
    cog_y = cog_y / 3;
    
    if (cog_x > vertex1[0] && cog_x > vertex1[9] && cog_y > vertex1[1] && cog_y > vertex1[10])
    {
        return true;
    }

    return false;
}
