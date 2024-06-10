#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "geometry_primitives.h"
#include <iostream>
#include <vector>
#include "camera.h"
#include "texture.h"
#include "texture_cube.h"
#include "math_utils.h"
#include "glm/ext.hpp"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window,float deltatime);

bool isWindowed = true;
bool isKeyboardProcessed[1024] = {0};
bool interpolation_slerp = false;
// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(4.0f, 1.0f, -4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool interpolation_linear = false;

float saved_yaw = 0.0f;
float saved_pitch = 0.0f;
glm::vec3 saved_Position;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float mixture = 0.5f;

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // define normal shader and skybox shader.
    Shader shader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like
    Shader skyboxShader("../shaders/shader_skybox.vs","../shaders/shader_skybox.fs");


    // TODO : define required VAOs(textured cube, skybox, quad)
    // data are defined in geometry_primitives.h
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GEOMETRY_PRIMITIVES_H::cube_positions_colors), GEOMETRY_PRIMITIVES_H::cube_positions_colors, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);




    unsigned int daytexture, nighttexture;

    std::vector<std::string> faces
    {
        "../resources/Sky Textures/right.jpg",
            "../resources/Sky Textures/left.jpg",
            "../resources/Sky Textures/top.jpg",
            "../resources/Sky Textures/bottom.jpg",
            "../resources/Sky Textures/front.jpg",
            "../resources/Sky Textures/back.jpg"
    };
    std::vector<std::string> faces_night
    {
        "../resources/Night Sky Textures/nightRight.png",
            "../resources/Night Sky Textures/nightLeft.png",
            "../resources/Night Sky Textures/nightTop.png",
            "../resources/Night Sky Textures/nightBottom.png",
            "../resources/Night Sky Textures/nightBack.png",
            "../resources/Night Sky Textures/nightFront.png"
    };

    TEXTURE_CUBE_H::CubemapTexture skybox(faces, faces_night);
    

    unsigned int skyboxVBO, skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GEOMETRY_PRIMITIVES_H::skybox_positions), GEOMETRY_PRIMITIVES_H::skybox_positions, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    skyboxShader.use();
    skyboxShader.setFloat("mixture", mixture);
    skyboxShader.setInt("textureID", 0);
    skyboxShader.setInt("textureID_night", 1);
    //skyboxShader.setInt("texture1",0);
    //skyboxShader.setInt("texture2", 0);
    // ... set view and projection matrix

    unsigned int quadVBO, quadVAO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GEOMETRY_PRIMITIVES_H::skybox_positions), GEOMETRY_PRIMITIVES_H::skybox_positions, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 6, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  1.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f,  2.2f, -2.5f),
        glm::vec3(-3.8f,  2.0f, -12.3f),
        glm::vec3( 2.4f,  1.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  1.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    const int n_grass = 1000;
    float grassGroundSize = 20;
    glm::vec3 grassPositions[n_grass];

    // positions of our grasses
    for(int i=0; i<n_grass; i++){
        float s = grassGroundSize/2;
        float x = getRandomValueBetween(-s, s);
        float z = getRandomValueBetween(-s, s);
        grassPositions[i].x = x;
        grassPositions[i].y = 0.5f;
        grassPositions[i].z = z;
    }


    // TODO : define textures (container, grass, grass_ground) & cubemap textures (day, night)
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("../resources/container.jpg", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int texturegrass;
    // texture 1
    // ---------
    glGenTextures(1, &texturegrass);
    glBindTexture(GL_TEXTURE_2D, texturegrass);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int widthgrass, heightgrass, nrChannelsgrass;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* datagrass = stbi_load("../resources/grass.png", &widthgrass, &heightgrass, &nrChannelsgrass, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthgrass, heightgrass, 0, GL_RGBA, GL_UNSIGNED_BYTE, datagrass);
    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int textureground;
    // texture 1
    // ---------
    glGenTextures(1, &textureground);
    glBindTexture(GL_TEXTURE_2D, textureground);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int widthground, heightground, nrChannelsground;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* dataground = stbi_load("../resources/grass_ground.jpg", &widthground, &heightground, &nrChannelsground, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthground, heightground, 0, GL_RGB, GL_UNSIGNED_BYTE, dataground);
    glGenerateMipmap(GL_TEXTURE_2D);


    // TODO : set texture & skybox texture uniform value (initialization)
    // e.g) shader.use(), shader.setInt("texture", 0);


    // render loop
    // -----------

    while (!glfwWindowShouldClose(window)) {


        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        if (interpolation_linear)
        {
            camera.linear(saved_pitch, saved_yaw, saved_Position, deltaTime, interpolation_linear, interpolation_slerp);
            processInput(window, deltaTime);
        }
        if (interpolation_slerp)
        {
            camera.linear(saved_pitch, saved_yaw, saved_Position, deltaTime, interpolation_linear, interpolation_slerp);
            processInput(window, deltaTime);
        }
        else
            processInput(window, deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        /////////////////////////////////////////////////////
        // TODO : Main Rendering Loop
        /////////////////////////////////////////////////////

        // (1) render boxes(cube) using normal shader.
        // (2) render ground(quad) using normal shader.
        // (3) render billboard grasses(quad) using normal shader.
        // (4) render skybox using skybox shader.
        
        
        // rendering pseudo-code

        // update projection, view matrix to shader
        // for each model:
        //      bind VAO, texture
        //      for each entity that belongs to the model:
        //          update model(transformation) matrix to shader
        //          glDrawArrays or glDrawElements


        // pass projection matrix to shader (note that in this case it could change every frame)
        
        
        //std::string str2 = std::to_string(mixture);
        //std::cout << str2 << std::endl;
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        shader.setMat4("projection", projection);
        glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        shader.setMat4("view", view);

        //glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        skyboxShader.setFloat("mixture", mixture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID_night);
        glm::mat4 view2 = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        skyboxShader.setMat4("view", view2);
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        //glDepthFunc(GL_LESS);


        //std::cout << glm::to_string(camera.Position) << std::endl;
        glBindVertexArray(VAO);
        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        for (unsigned int i = 0; i < 10; i++)
        {
            // locate the cubes where you want!
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 5.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(15.0f, 13.0f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureground);

        glm::mat4 model = glm::mat4(1.0f);
        
        
        float angle = 90.0f;

        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f,100.0f, 0.0f));
        shader.setMat4("model", model);
        //shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturegrass);

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (unsigned int i = 0; i < 1000; i++)
        {
            // locate the cubes where you want!
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, grassPositions[i]);
            float y_diff = grassPositions[i][2] - camera.Position[2];
            float x_diff = grassPositions[i][0] - camera.Position[0];
            //std::cout << x_diff << std::endl;
            //std::cout << y_diff << std::endl;
            angle = glm::atan(x_diff, y_diff);
            //std::cout << y_diff << std::endl;
            model = glm::rotate(model, (angle), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0f, 01.0f, 1.0f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, float deltaTime)
{

    // TODO : make camera movable (WASD) & increase or decrease dayFactor(press O: increase, press P: decrease)

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    #define GLM_ENABLE_EXPERIMENTAL
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (interpolation_linear==false)
    {

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.Position += cameraSpeed * camera.Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.Position -= cameraSpeed * camera.Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            saved_pitch = camera.Pitch;
            saved_yaw = camera.Yaw;
            saved_Position = camera.Position;
            std::cout << saved_pitch << std::endl;
            std::cout << saved_yaw << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            if (mixture <= 1.0f)
                mixture += 0.1f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
            if (mixture >= 0.0f)
                mixture -= 0.1f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)

        {
            interpolation_linear = true;

        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)

        {
            interpolation_linear = true;
            interpolation_slerp = true;
        }
    }
 }

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // TODO: calculate how much cursor have moved, rotate camera proportional to the value, using ProcessMouseMovement.

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset,yoffset);




}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
