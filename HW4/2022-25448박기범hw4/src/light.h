#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


class DirectionalLight {
public:
	float azimuth;
	float elevation;
	glm::vec3 lightDir; // direction of light. If elevation is 90, it would be (0,-1,0)
	glm::vec3 lightColor; // this is I_d (I_s = I_d, I_a = 0.3 * I_d)

	DirectionalLight(float azimuth, float elevation, glm::vec3 lightColor) {
		this->azimuth = azimuth;
		this->elevation = elevation;
		updateLightDir();
		this->lightColor = lightColor;
	}

	DirectionalLight(glm::vec3 lightDir, glm::vec3 lightColor) {
		this->lightDir = lightDir;
		this->lightColor = lightColor;
	}

	glm::mat4 getViewMatrix(glm::vec3 cameraPosition) {
		// directional light has no light position. Assume fake light position depending on camera position.
		float lightDistance = 15.0f;
		glm::vec3 lightPos = cameraPosition - this->lightDir * lightDistance;
		return glm::lookAt(lightPos, cameraPosition, glm::vec3(0, 1, 0));
	}

	glm::mat4 getProjectionMatrix() {
		// For simplicity, just use static projection matrix. (Actually we have to be more accurate with considering camera's frustum)
		return glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 50.0f);
	}

	void updateLightDir() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(glm::rotate(model, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(elevation), glm::vec3(1.0f, 0.0f, 0.0f));
		lightDir = model * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);


	}

	// Processes input received from a mouse input system. Expects the offset value in both the x(azimuth) and y(elevation) direction.
	void processKeyboard(float xoffset, float yoffset)
	{
		// TODO:
		// set elevation between 15 to 80 (degree)!

	}
};

#endif