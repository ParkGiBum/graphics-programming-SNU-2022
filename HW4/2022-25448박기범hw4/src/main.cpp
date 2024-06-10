#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "opengl_utils.h"
#include "geometry_primitives.h"
#include <iostream>
#include <vector>
#include "camera.h"
#include "texture.h"
#include "texture_cube.h"
#include "model.h"
#include "mesh.h"
#include "scene.h"
#include "math_utils.h"
#include "light.h"
std::vector<glm::mat4> getLightSpaceMatrices(DirectionalLight sun);
glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane, DirectionalLight sun);
std::vector<float> shadowCascadeLevels{ 50.0 / 50.0f, 50.0 / 25.0f, 50.0 / 10.0f };
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, DirectionalLight* sun);

bool isWindowed = true;
bool isKeyboardDone[1024] = { 0 };
void renderQuad();
// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int SHADOW_WIDTH = 2048 * 4;
const unsigned int SHADOW_HEIGHT = 2048 * 4;
const float planeSize = 15.f;

// camera
Camera camera(glm::vec3(0.0f, 0.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float useLighting = 0.0f;
float useShadow = 0.0f;
float usepcf = 0.0f;
float usecsm = 0.0f;
bool useNormalMap = false;
bool useSpecular = false;

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
    Shader lightingShader("../shaders/shader_lighting.vs", "../shaders/shader_lighting.fs"); // you can name your shader files however you like
    Shader shadowShader("../shaders/shadow.vs", "../shaders/shadow.fs");
    Shader skyboxShader("../shaders/shader_skybox.vs", "../shaders/shader_skybox.fs");
    Shader debugDepthQuad("../shaders/debug.vs", "../shaders/debug.fs");

    // define models
    // There can be three types 
    // (1) diffuse, specular, normal : brickCubeModel
    // (2) diffuse, normal only : boulderModel
    // (3) diffuse only : grassGroundModel
    Model brickCubeModel = Model("../resources/brickcube/brickcube.obj");
    brickCubeModel.diffuse = new Texture("../resources/brickcube/brickcube_d.png");
    brickCubeModel.specular = new Texture("../resources/brickcube/brickcube_s.png");
    brickCubeModel.normal = new Texture("../resources/brickcube/brickcube_n.png");

    Model boulderModel("../resources/boulder/boulder.obj");
    boulderModel.diffuse = new Texture("../resources/boulder/boulder_d.png");
    boulderModel.normal = new Texture("../resources/boulder/boulder_n.png");

    Model grassGroundModel = Model("../resources/plane.obj");
    grassGroundModel.diffuse = new Texture("../resources/grass_ground.jpg");
    grassGroundModel.ignoreShadow = true;


    // TODO: Add more models (barrels, fire extinguisher) and YOUR own model
    Model barrelModel = Model("../resources/barrel/barrel.obj");
    barrelModel.diffuse = new Texture("../resources/barrel/barrel_d.png");
    barrelModel.specular = new Texture("../resources/barrel/barrel_s.png");
    barrelModel.normal = new Texture("../resources/barrel/barrel_n.png");


    Model fireExtModel = Model("../resources/FireExt/FireExt.obj");
    fireExtModel.diffuse = new Texture("../resources/FireExt/FireExt_d.jpg");
    fireExtModel.specular = new Texture("../resources/FireExt/FireExt_s.jpg");
    fireExtModel.normal = new Texture("../resources/FireExt/FireExt_n.jpg");
    //Model yourOwnModel;

    Model gun = Model("../resources/gun/13463_Australian_Cattle_Dog_v3.obj");
    gun.diffuse = new Texture("../resources/gun/Australian_Cattle_Dog_dif.jpg");
    //gun.specular = new Texture("../resources/gun/handgun_S.jpg");
    //gun.normal = new Texture("../resources/gun/handgun_N.jpg");


    // Add entities to scene.
    // you can change the position/orientation.
    Scene scene;
    scene.addEntity(new Entity(&brickCubeModel, glm::mat4(1.0)));
    scene.addEntity(new Entity(&brickCubeModel, glm::translate(glm::vec3(-3.5f, 0.0f, -2.0f)) * glm::rotate(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
    scene.addEntity(new Entity(&brickCubeModel, glm::translate(glm::vec3(1.0f, 0.5f, -3.0f)) * glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f))));
    scene.addEntity(new Entity(&barrelModel, glm::vec3(2.5f, 0.0f, -2.0f), 0, 0, 0, 0.1f));

    glm::mat4 planeWorldTransform = glm::mat4(1.0f);
    planeWorldTransform = glm::scale(planeWorldTransform, glm::vec3(planeSize));
    planeWorldTransform = glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)) * planeWorldTransform;
    scene.addEntity(new Entity(&grassGroundModel, planeWorldTransform));

    scene.addEntity(new Entity(&fireExtModel, glm::vec3(2, -1, 0), 0.0f, 180.0f, 0.0f, 0.002f));
    scene.addEntity(new Entity(&boulderModel, glm::vec3(-5, 0, 2), 0.0f, 180.0f, 0.0f, 0.1));
    scene.addEntity(new Entity(&gun, glm::vec3(-1.0, 0.0, 0.0), 0.0f, 180.0f, 0.0f, 0.02));
    // add your model's entity here!



    // define depth texture
    DepthMapTexture depth = DepthMapTexture(SHADOW_WIDTH, SHADOW_HEIGHT);


    // skybox
    std::vector<std::string> faces
    {
        "../resources/skybox/right.jpg",
        "../resources/skybox/left.jpg",
        "../resources/skybox/top.jpg",
        "../resources/skybox/bottom.jpg",
        "../resources/skybox/front.jpg",
        "../resources/skybox/back.jpg"
    };
    CubemapTexture skyboxTexture = CubemapTexture(faces);
    unsigned int VAOskybox, VBOskybox;
    getPositionVAO(skybox_positions, sizeof(skybox_positions), VAOskybox, VBOskybox);



    lightingShader.use();
    lightingShader.setInt("material.diffuseSampler", 0);
    lightingShader.setInt("material.specularSampler", 1);
    lightingShader.setInt("material.normalSampler", 2);
    //lightingShader.setInt("material.depthMapSampler", 3);
    lightingShader.setFloat("material.shininess", 64.f);    // set shininess to constant value.
    lightingShader.setInt("shadowMap", 3);
    skyboxShader.use();
    skyboxShader.setInt("skyboxTexture1", 0);


    DirectionalLight sun(210.0f, 30.0f, glm::vec3(0.8f));
    //debugDepthQuad.use();
    //debugDepthQuad.setInt("depthMap", 0);
    float oldTime = 0;
    while (!glfwWindowShouldClose(window))// render loop
    {
        float currentTime = glfwGetTime();
        float dt = currentTime - oldTime;
        deltaTime = dt;
        oldTime = currentTime;

        // input

        processInput(window, &sun);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sun.updateLightDir();

        // TODO : 
        // (1) render shadow map!
            // framebuffer: shadow frame buffer(depth.depthMapFBO)
            // shader : shadow.fs/vs    
        shadowShader.use();
        glm::mat4 lightProjection = sun.getProjectionMatrix();
        glm::mat4 lightView = sun.getViewMatrix(camera.Position);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView; 
        std::vector<glm::mat4> lightSpaceMatrices = getLightSpaceMatrices(sun);
        shadowShader.setMat4("lightSpaceMatrix", lightProjection * lightView);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depth.depthMapFBO);
        for (size_t i = 0; i < lightSpaceMatrices.size(); ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightSpaceMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
        //glActiveTexture(GL_TEXTURE0);

        map<Model*, vector<Entity*>>::iterator it2;

        for (it2 = scene.entities.begin(); it2 != scene.entities.end(); it2++) {
            for (int i = 0; i < it2->second.size(); i++)
            {
            
                if (useShadow && it2->first->ignoreShadow==false)
                {
                    it2->first->bind();
                    //lightingShader.setMat4("world", it->second[i]->getModelMatrix());
                    //lightingShader.setFloat("useNormalMap", 0.0f);
                    //lightingShader.setFloat("useSpecularMap", 0.0f);
                    if (it2->first->specular) {
                        //lightingShader.setFloat("useSpecularMap", 1.0f);
                    }
                    if (it2->first->normal) {
                        //lightingShader.setFloat("useNormalMap", useNormalMap);
                    }
                   
                    shadowShader.setMat4("model", it2->second[i]->getModelMatrix());

                    glDrawElements(GL_TRIANGLES, it2->first->mesh.indices.size(), GL_UNSIGNED_INT, 0);
                }

            }
        }

         
        // (2) render objects in the scene!
            // framebuffer : default frame buffer(0)
            // shader : shader_lighting.fs/vs
        // 
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        lightingShader.use();
        lightingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setVec3("light.dir", sun.lightDir);
        lightingShader.setVec3("light.color", sun.lightColor);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("Material.shininess", 64.0f);
        lightingShader.setInt("cascadeCount", shadowCascadeLevels.size());
        for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
        {
            lightingShader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
        }

        glm::mat4 model = glm::mat4(1.0f);
        //std::cout << sun.lightDir[0]<<" "<< sun.lightDir[1] << " " << sun.lightDir[2] << endl;
        lightingShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
        lightingShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setMat4("world", model);

        lightingShader.setFloat("useSpecularMap", 0.0f);

        lightingShader.setFloat("useShadow", useShadow);
        lightingShader.setFloat("usepcf", usepcf);
        lightingShader.setFloat("usecsm", usecsm);
        lightingShader.setFloat("useLighting", useLighting);
        //first = model
        //second = entity
        map<Model*, vector<Entity*>>::iterator it;// = scene.entities.begin();
        for (it = scene.entities.begin(); it != scene.entities.end(); it++) {
            for (int i = 0; i < it->second.size(); i++)
            {   
                it->first->bind();
                lightingShader.setMat4("world", it->second[i]->getModelMatrix());
                lightingShader.setFloat("useNormalMap", 0.0f);
                lightingShader.setFloat("useSpecularMap", 0.0f);
                if (it->first->specular) {
                    lightingShader.setFloat("useSpecularMap", 1.0f);
                }
                if (it->first->normal) {
                    lightingShader.setFloat("useNormalMap", useNormalMap);
                }
                glActiveTexture(GL_TEXTURE3);//assigned shadow map to 3
                glBindTexture(GL_TEXTURE_2D, depth.ID);
                glDrawElements(GL_TRIANGLES, it->first->mesh.indices.size(), GL_UNSIGNED_INT, 0);

            }
        }








        // use skybox Shader
        skyboxShader.use();
        glDepthFunc(GL_LEQUAL);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // render a skybox
        glBindVertexArray(VAOskybox);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        float near_plane = 0.1f, far_plane = 50.0f;
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth.ID);
        //renderQuad();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
float tk;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, DirectionalLight* sun)
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
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        sun->azimuth = sun->azimuth - deltaTime * 10.0;
    
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        sun->azimuth = sun->azimuth + deltaTime * 10.0;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        sun->elevation = sun->elevation + deltaTime * 10.0;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        sun->elevation = sun->elevation - deltaTime * 10.0;
    sun->azimuth = clamp(sun->azimuth,0.0,360.0) ;
    sun->elevation = clamp(sun->elevation,15.0,80.0) ;
    //std::cout<<(sun->elevation)<<endl;
    //std::cout << useShadow << endl;
    float t = 20.0f * deltaTime;
    tk = tk - t;
    tk = clamp(tk, 0.0, 3.0);
    // TODO : 
    // Arrow key : increase, decrease sun's azimuth, elevation with amount of t.
    // key 1 : toggle using normal map
    // key 2 : toggle using shadow
    // key 3 : toggle using whole lighting
    if (tk == 0.0) {
        tk = 3.0f;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            if (useLighting == 1.0) {
                useLighting = 0.0f;
            }
            else {
                useLighting = 1.0f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            if (useShadow == 1.0) {
                useShadow = 0.0f;
            }
            else {
                useShadow = 1.0f;
            }
        }if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            if (useNormalMap == 1.0) {
                useNormalMap = 0.0f;
            }
            else {
                useNormalMap = 1.0f;
            }
        }if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            if (usepcf == 1.0) {
                usepcf = 0.0f;
            }
            else {
                usepcf = 1.0f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            if (usecsm == 1.0) {
                usecsm = 0.0f;
            }
            else {
                usecsm = 1.0f;
            }
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




unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane, DirectionalLight sun)
{
    const auto proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, nearPlane,farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj * camera.GetViewMatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + sun.lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

std::vector<glm::mat4> getLightSpaceMatrices(DirectionalLight sun)
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(0.1f, shadowCascadeLevels[i],sun));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i],sun));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], 50.0f,sun));
        }
    }
    return ret;
}