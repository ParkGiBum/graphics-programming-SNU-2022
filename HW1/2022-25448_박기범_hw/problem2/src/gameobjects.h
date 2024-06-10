#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H
#include <glm/glm.hpp>
#include "math_utils.h"
struct Entity
{
    float scale;
    float rotate;
    float dropSpeed;
    float rotateSpeed;
    glm::vec3 position;
};
struct Bar
{   
    float length;
    float xPosition;
    float speed;
};
Entity getRandomEntity()
{
    Entity e;
    e.scale = getRandomValueBetween(0.04f, 0.1f);
    e.rotate = getRandomValueBetween(0.1f, 3);
    e.dropSpeed = getRandomValueBetween(0.1f, 0.4f);
    e.rotateSpeed = getRandomValueBetween(0.1f, 0.4f);
    e.position = glm::vec3(getRandomValueBetween(-0.9f, 0.9f),1,0);
    return e;
}
#endif