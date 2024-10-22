#ifndef _SCENE_H_
#define _SCENE_H_
#include <SDL.h>
#include <vector>
#include <memory>
#include <glm.hpp>
#include "Object.h"
#include "Material.h"
#include "Mesh.h"
#include "Flame.h"


//contains references to all objects, flames, shaders, materials and meshes in the scene
//as well as having control over the camera and light position
//has functions which cause the whoel scene to be updated and drawn from a single call
class Scene
{
private:
	std::vector<Object*> m_objects;
	std::vector<Flame*> m_fires;
	Shader *m_logShader;
	Shader *m_pointsShader;

	Shader* m_point_small_Shader;

	Shader *m_grassShader;
	Material *m_logMat;
	Material *m_grassMat;
	Material *m_particleMat;
	Mesh *m_logMesh;
	Mesh *m_grassMesh;
	glm::vec3 m_cameraRotation;
	glm::vec4 m_lightPosition;
	Uint32 m_deltaTime;
	Uint32 m_previousTime;
	bool m_fireUp;
	bool m_rotateRight;
	bool m_rotateLeft;

	int time = 0;
	
	
public:
	bool pause;

	int t = 0;
	Scene(void);
	void Draw();
	void Update(bool pause);
	void SetDeltaTime(unsigned int _deltaTime) { t = t + 1; m_deltaTime = _deltaTime; float time = _deltaTime; m_pointsShader->setFloat("time", time); std::cout << "test" << std::endl; for (int i = 0; i < NUMBEROFPARTICLES; i++) { }; }
	void SetRotateRight(bool _rotate){m_rotateRight = _rotate;}
	void SetRotateleft(bool _rotate){m_rotateLeft = _rotate;}
	void go_f(bool _rotate) { m_rotateLeft = _rotate; }
	void go_b(bool _rotate) { m_rotateLeft = _rotate; }
};

#endif