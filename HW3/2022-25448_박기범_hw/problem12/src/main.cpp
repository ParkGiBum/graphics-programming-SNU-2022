#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))  
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))  
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "resource_utils.h"
#include <iostream>
#include <vector>
#include "camera.h"
#include "math_utils.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool isKeyboardDone[1024] = {0};
bool outer = false;
bool is_l = false;
// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


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
    

    // build and compile our shader program
    // ------------------------------------
    // TODO: define 3 shaders
    // (1) geometry shader for spline render.
    // (2) simple shader for spline's outer line render.
    // (optional) (3) tessellation shader for bezier surface.
    Shader bzshader("../shaders/bezier_surface/tess.vs", "../shaders/bezier_surface/tess.fs", "../shaders/bezier_surface/tess.gs", "../shaders/bezier_surface/TCS.glsl", "../shaders/bezier_surface/TES.glsl");
    Shader spshader("../shaders/splines/spline_shader.vs", "../shaders/splines/spline_shader.fs", "../shaders/splines/spline_shader.gs");
    Shader outerShader("../shaders/outer_line_shader.vs", "../shaders/outer_line_shader.fs");

    // TODO : load requied model and save data to VAO. 
    // Implement and use loadSplineControlPoints/loadBezierSurfaceControlPoints in resource_utils.h
    VAO* VAO_spcp = RESOURCE_UTILS_H::loadSplineControlPoints("../resources/spline_control_point_data/spline_complex.txt");
    VAO* VAO_simp = RESOURCE_UTILS_H::loadSplineControlPoints("../resources/spline_control_point_data/spline_simple.txt");
    VAO* VAO_sp_u = RESOURCE_UTILS_H::loadSplineControlPoints("../resources/spline_control_point_data/spline_u.txt");

    VAO* VAO_bztp = RESOURCE_UTILS_H::loadBezierSurfaceControlPoints("../resources/bezier_surface_data/teapot.bpt");
    VAO* VAO_bzgb = RESOURCE_UTILS_H::loadBezierSurfaceControlPoints("../resources/bezier_surface_data/gumbo.bpt");
    VAO* VAO_bzsp = RESOURCE_UTILS_H::loadBezierSurfaceControlPoints("../resources/bezier_surface_data/sphere.bpt");
    VAO* VAO_bzht = RESOURCE_UTILS_H::loadBezierSurfaceControlPoints("../resources/bezier_surface_data/heart.bpt");

    // render loop
    // -----------
    float oldTime = 0;
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float dt = currentTime - oldTime;
        deltaTime = dt;

        oldTime = currentTime;

        // input
        // -----
        processInput(window);

        

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // TODO : render splines
        // (1) render simple spline with 4 control points for Bezier, Catmull-Rom and B-spline.
        // (2) render 'u' using Bezier spline
        // (3) render loop using Catmull-Rom spline and B-spline.
        // You have to also render outer line of control points!
        spshader.use();
        spshader.setBool("B_S", false);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom ), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        spshader.setMat4("projection", projection);
        glm::vec3 scale = glm::vec3(0.2f, 0.2f, 0.2f);
        glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, scale);
        model = glm::translate(model, { 25.0,0.0f,0.0 });
        spshader.setMat4("model", model);
        spshader.setMat4("view", view);
        spshader.setFloat("opt", 1.0f);
        spshader.setFloat("loc", 0.0f);
        glBindVertexArray(VAO_sp_u->ID);
        glDrawArrays(GL_LINES_ADJACENCY, 0, 68);
        glBindVertexArray(0);

        if (outer) {
            outerShader.use();
            projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            outerShader.setMat4("projection", projection);
            scale = glm::vec3(0.2f, 0.2f, 0.2f);
            view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
            model = glm::mat4(1.0f);
            model = glm::scale(model, scale);
            model = glm::translate(model, { 25.0,0.0f,0.0 });
            outerShader.setMat4("model", model);
            outerShader.setMat4("view", view);
            glBindVertexArray(VAO_sp_u->ID);
            glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 68);
            glBindVertexArray(0);
        }


        spshader.use();
        model = glm::mat4(1.0f);
        spshader.setMat4("model", model);
        glBindVertexArray(VAO_simp->ID);
        spshader.setBool("B_S", false);
        spshader.setFloat("loc", 0.0f);
        glBindVertexArray(VAO_simp->ID);
        glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
        glBindVertexArray(0);

        if (outer) {
            outerShader.use();
            outerShader.setMat4("projection", projection);
            outerShader.setMat4("model", model);
            outerShader.setMat4("view", view);
            glBindVertexArray(VAO_simp->ID);
            glDrawArrays(GL_LINE_STRIP, 0, 4);
            glBindVertexArray(0);
        }
        spshader.use();

        spshader.setMat4("model", model);
        spshader.setBool("B_S", true);
        glBindVertexArray(VAO_simp->ID);
        model = glm::translate(model, {1.5f,0,0});
        spshader.setMat4("model", model);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 4);
        glBindVertexArray(0);

        if (outer) {
            outerShader.use();
            outerShader.setMat4("projection", projection);
            outerShader.setMat4("model", model);
            outerShader.setMat4("view", view);
            glBindVertexArray(VAO_simp->ID);
            glDrawArrays(GL_LINE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

        spshader.use();

        
        spshader.setBool("B_S", false);
        spshader.setBool("cat", true);
        glBindVertexArray(VAO_simp->ID);
        model = glm::translate(model, { 2.0f,0,0 });
        spshader.setMat4("model", model);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 4);
        glBindVertexArray(0);
        if (outer) {
            outerShader.use();
            outerShader.setMat4("projection", projection);
            outerShader.setMat4("model", model);
            outerShader.setMat4("view", view);
            glBindVertexArray(VAO_simp->ID);
            glDrawArrays(GL_LINE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

        spshader.use();


        spshader.setBool("cat", false);
        spshader.setBool("B_S", true);
        glBindVertexArray(VAO_spcp->ID);
        model = glm::translate(model, { 4.0f,0,0 });
        spshader.setMat4("model", model);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 23);
        glBindVertexArray(0);

        if (outer) {
            outerShader.use();
            outerShader.setMat4("projection", projection);
            outerShader.setMat4("model", model);
            outerShader.setMat4("view", view);
            glBindVertexArray(VAO_spcp->ID);
            glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 23);
            glBindVertexArray(0);
        }
        spshader.use();


        spshader.setBool("B_S", false);
        spshader.setBool("cat", true);
        glBindVertexArray(VAO_spcp->ID);
        model = glm::translate(model, { 4.0f, 0, 0 });
        spshader.setMat4("model", model);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 23);
        glBindVertexArray(0);
        spshader.setBool("cat", false);

        if (outer) {
            outerShader.use();
            outerShader.setMat4("projection", projection);
            outerShader.setMat4("model", model);
            outerShader.setMat4("view", view);
            glBindVertexArray(VAO_spcp->ID);
            glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 23);
            glBindVertexArray(0);
        }



        // (Optional) TODO : render Bezier surfaces using tessellation shader.
        


        bzshader.use();
        if(is_l)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        bzshader.setMat4("projection", projection);
        view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        model = glm::mat4(1.0f); 
        model = glm::translate(model, { 0.0f, -3.0f, 0 });
        bzshader.setMat4("model", model);
        bzshader.setMat4("view", view);
        bzshader.setVec3("cameraPosition", camera.Position);
        glPatchParameteri(GL_PATCH_VERTICES,16);
        glBindVertexArray(VAO_bzsp->ID);
        glDrawArrays(GL_PATCHES, 0, 8*16 );
        glBindVertexArray(0);

        bzshader.use();
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        bzshader.setMat4("projection", projection);
        view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        model = glm::mat4(1.0f);
        scale = glm::vec3(0.4f, 0.4f, 0.4f);
        
        model = glm::translate(model, { 2.0f, -3.0f, 0 });
        model = glm::scale(model, scale);
        bzshader.setMat4("model", model);
        bzshader.setMat4("view", view);
        bzshader.setVec3("cameraPosition", camera.Position);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glBindVertexArray(VAO_bztp->ID);
        glDrawArrays(GL_PATCHES, 0, 32 * 16);
        glBindVertexArray(0);


        bzshader.use();
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        bzshader.setMat4("projection", projection);
        view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        model = glm::mat4(1.0f);
        scale = glm::vec3(0.1f, 0.1f, 0.1f);

        model = glm::translate(model, { 4.0f, -3.0f, 0 });
        model = glm::scale(model, scale);
        bzshader.setMat4("model", model);
        bzshader.setMat4("view", view);
        bzshader.setVec3("cameraPosition", camera.Position);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glBindVertexArray(VAO_bzgb->ID);
        glDrawArrays(GL_PATCHES, 0, 128 * 16);
        glBindVertexArray(0);

        bzshader.use();
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        bzshader.setMat4("projection", projection);
        view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        model = glm::mat4(1.0f);
        scale = glm::vec3(0.5f, 0.5f, 0.5f);

        model = glm::translate(model, { 8.0f, -3.0f, 0 });
        model = glm::scale(model, scale);
        bzshader.setMat4("model", model);
        bzshader.setMat4("view", view);
        bzshader.setVec3("cameraPosition", camera.Position);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glBindVertexArray(VAO_bzht->ID);
        glDrawArrays(GL_PATCHES, 0, 2 * 16);
        glBindVertexArray(0);




        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1,&VAO);
    //glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        if (outer == false) {
            outer = true;
        }
        else {
            outer = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        if (is_l == false) {
            is_l = true;
        }
        else {
            is_l = false;
        }
    }
        
    // TODO : 
    // (1) (for spline) if we press key 9, toggle whether to render outer line.
    // (2) (Optional, for Bezier surface )if we press key 0, toggle GL_FILL and GL_LINE.
    
    
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}