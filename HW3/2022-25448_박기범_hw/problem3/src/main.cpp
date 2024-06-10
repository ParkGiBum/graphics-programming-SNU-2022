#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "class.h"

#include <iostream>
#include <cmath>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
int obj2 = 1;
int level = 0;
const GLfloat distance = 7.f;
const GLfloat theta = atan2(1.f, 0.7f);
const glm::mat4 modelMatrix = glm::mat4(1.0);
const glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
GLfloat phi = 0.f;
glm::vec3 cameraPosition = distance * glm::vec3(0.f, glm::cos(theta), glm::sin(theta));
glm::mat4 viewMatrix = glm::lookAt(cameraPosition, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

// light
const glm::vec3 lightPosition(10.0, 10.0, 10.0);
const glm::vec3 lightColor(0.3, 0.7, 0.9);

// object
const glm::vec3 objectColor(0.9, 0.7, 0.7);
unsigned int VAO, VBO, EBO;
unsigned int VAOd, VBOd, EBOd;
unsigned int VAOs, VBOs, EBOs;
object* obj = cube();
object* obj_donut = donut();
object* obj_star = star();

void setCameraPosition()
{
	cameraPosition = distance * glm::vec3(glm::sin(theta) * glm::sin(phi), glm::cos(theta), glm::sin(theta) * glm::cos(phi));
}

void setViewMatrix()
{
	viewMatrix = glm::lookAt(cameraPosition,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f));
}

void update(GLfloat dt)
{
	phi += dt;
	setCameraPosition();
	setViewMatrix();
}

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Catmull-Clark Subdivision", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// ------------------------------------
	Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj->vertices->size(), obj->vertices->data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->vertexIndices->size(), obj->vertexIndices->data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);




	

	glGenVertexArrays(1, &VAOd);
	glGenBuffers(1, &VBOd);
	glGenBuffers(1, &EBOd);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAOd);

	glBindBuffer(GL_ARRAY_BUFFER, VBOd);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj_donut->vertices->size(), obj_donut->vertices->data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOd);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj_donut->vertexIndices->size(), obj_donut->vertexIndices->data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);





	glGenVertexArrays(1, &VAOs);
	glGenBuffers(1, &VBOs);
	glGenBuffers(1, &EBOs);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj_star->vertices->size(), obj_star->vertices->data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj_star->vertexIndices->size(), obj_star->vertexIndices->data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	


	float oldTime = 0.f;
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// render
		// ------
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render the triangle
		ourShader.use();
		float currentTime = glfwGetTime();
		float dt = currentTime - oldTime;
		oldTime = currentTime;
		update(dt);

		// for vertex shader
		ourShader.setMat4("model", modelMatrix);
		ourShader.setMat4("view", viewMatrix);
		ourShader.setMat4("projection", projectionMatrix);
		ourShader.setVec3("worldLightPos", lightPosition);
		ourShader.setVec3("worldCameraPos", cameraPosition);

		// for pixel shader
		ourShader.setVec3("lightColor", lightColor);
		ourShader.setVec3("objectColor", objectColor);
		if (obj2 == 1) {
			//obj = catmull_clark(obj);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj->vertices->size(), obj->vertices->data(), GL_STATIC_DRAW);
			//glBindVertexArray(VAO);
			//glDrawElements(GL_TRIANGLES, obj->vertexIndices->size(), GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->vertexIndices->size(), obj->vertexIndices->data(), GL_STATIC_DRAW);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, obj->vertexIndices->size(), GL_UNSIGNED_INT, 0);
		}
		if (obj2 == 2) {
			//glBindVertexArray(VAOd);
			//glBindBuffer(GL_ARRAY_BUFFER, VBOd);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj_donut->vertices->size(), obj_donut->vertices->data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, VBOd);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj_donut->vertices->size(), obj_donut->vertices->data(), GL_STATIC_DRAW);
			//glBindVertexArray(VAOs);
			//glDrawElements(GL_TRIANGLES, obj_donut->vertexIndices->size(), GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOd);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj_donut->vertexIndices->size(), obj_donut->vertexIndices->data(), GL_STATIC_DRAW);
			glBindVertexArray(VAOd);
			glDrawElements(GL_TRIANGLES, obj_donut->vertexIndices->size(), GL_UNSIGNED_INT, 0);
		}
		if (obj2 == 3) {
			
			//glBindVertexArray(VAOd);
			//glBindBuffer(GL_ARRAY_BUFFER, VBOd);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj_donut->vertices->size(), obj_donut->vertices->data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, VBOs);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj_star->vertices->size(), obj_star->vertices->data(), GL_STATIC_DRAW);
			//glBindVertexArray(VAOs);
			//glDrawElements(GL_TRIANGLES, obj_donut->vertexIndices->size(), GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj_star->vertexIndices->size(), obj_star->vertexIndices->data(), GL_STATIC_DRAW);
			glBindVertexArray(VAOs);
			glDrawElements(GL_TRIANGLES, obj_star->vertexIndices->size(), GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAOd);
	glDeleteBuffers(1, &VBOd);
	glDeleteVertexArrays(1, &VAOs);
	glDeleteBuffers(1, &VBOs);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		obj2 = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		obj2 = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		obj2 = 3;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if(obj2 == 1)
			obj = catmull_clark(obj);
		if(obj2 == 2)
			obj_donut = catmull_clark(obj_donut);
		if(obj2 == 3)
			obj_star = catmull_clark(obj_star);
	}	
		
	/* fill in the blank */
	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
