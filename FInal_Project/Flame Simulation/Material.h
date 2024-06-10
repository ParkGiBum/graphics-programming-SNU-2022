#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include "Shader.h"
#include <memory>
#include <string>
#include <SDL_image.h>
#include <iostream>
//material class for storing indexes of textures, a pointer to a shader and necessary view and projection matrices
//also contains function to ready the shader for drawing
class Material
{
private:
	GLuint m_colourTexture;
	GLuint m_normalTexture;
	Shader* m_shader;
	
	glm::mat4 m_viewMatrix;
	glm::vec3* m_cameraRotation;
	glm::vec4* m_lightPosition;
	glm::mat4 m_projMatrix;
public:
	float time;
	float forward;
	void settime() { m_shader->time = time;  }
	Material() { forward = 0; };
	Material(std::string _colourTexName, std::string _normalTexName, Shader* _shader, glm::vec3* _cameraRotation, glm::vec4* _lightPosition);

	void ReadyForDraw(glm::mat4 _modelMatrix);
};

#endif