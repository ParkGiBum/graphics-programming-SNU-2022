#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
float YAW         =  0.0f;
float PITCH       =  0.0f;
float SPEED       =  2.5f;
float SENSITIVITY =  0.1f;
float ZOOM        =  45.0f; //(FOV)


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position ;
    glm::vec3 Front;
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw = YAW;
    float Pitch = PITCH;
    float Roll = 0.0f;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom = 45.0f;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        // TODO : fill in
        Position = position;
        Up = up;
        Yaw = yaw;
        Pitch = pitch;
       
    }

    // Constructor with scalar values
    //Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    //{
        // TODO : fill in


    //}

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        // TODO : fill in


    }
    bool yaw_set = false;
    bool pitch_set = false;
    bool position_set = false;
    bool pitch_inverted = false;
    bool yaw_inverted = false;
    float po = 361.0f;
    float yo = 361.0f;
    bool halt = false;
    float added_time = 0.0f;
    glm::vec3 ps0;
    void linear(float savedpitch, float savedyaw , glm::vec3 savedposition,float deltaTime,bool &interpolation_linear,bool& slerp)
    {
        halt = true;

        double cy = cos(Yaw * 0.5);
        double sy = sin(Yaw * 0.5);
        double cp = cos(Pitch * 0.5);
        double sp = sin(Pitch * 0.5);
        double cr = cos(Roll * 0.5);
        double sr = sin(Roll * 0.5);
        glm::quat q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;


        //float pitch_offset = fmod(savedpitch,360) - fmod(Pitch, 360);
        float pitch_offset = fmod(savedpitch, 360) - fmod(Pitch, 360);
        added_time += deltaTime;
        if (pitch_offset * pitch_offset >  180.0f * 180.0f)
        {
            if (pitch_offset > 0.0f) {
                pitch_offset = pitch_offset - 360.0f;
            }
            else {
                pitch_offset = pitch_offset + 360.0f;
            }
        }
        float yaw_offset  = fmod(savedyaw, 360) - fmod(Yaw, 360);
        if (yaw_offset * yaw_offset > 180.0f * 180.0f )
        {
            if (yaw_offset > 0.0f) {
                yaw_offset = yaw_offset - 360.0f;
            }
            else {
                yaw_offset = yaw_offset + 360.0f;
            }

        }
        glm::vec3 postion_offset = savedposition - Position;
        if (po > 360.0f)
        {
            po = pitch_offset;
            yo = yaw_offset;
            ps0 = postion_offset;
            
        }
        std::cout << Pitch <<" , " << Yaw << std::endl;
        
        if (pitch_offset * pitch_offset > 1.0f  && pitch_set == false)
        {
            Pitch = Pitch + po * deltaTime * 2.0f;         
        }
        else {
            pitch_set = true;
        }
        if (yaw_offset * yaw_offset > 1.0f && yaw_set == false)
        {
            Yaw = Yaw + yo * deltaTime * 2.0f;
        }
        else
        {
            yaw_set = true;
        }

        if (postion_offset[0] * postion_offset[0] > 0.04f && postion_offset[1] * postion_offset[1] > 0.04f && postion_offset[2] * postion_offset[2] > 0.04f)
        {
            Position += ps0 * deltaTime * 2.0f;
        }
        else 
        {
            position_set = true;
        }



        if (pitch_set && yaw_set && position_set) {

            interpolation_linear = false;
            slerp = false;
            yaw_set = false;
            pitch_set = false;
            position_set = false;
            pitch_inverted = false;
            yaw_inverted = false;
            halt = false;
            po = 361.0f;
            yo = 361.0f;
        }
        else
        {
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
        }
        //std::cout << pitch_offset << std::endl;
        
    }


    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        // TODO : fill in

    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        // TODO : fill in
        // pitch between -89 and 89 degree
        float x_off = xoffset * SENSITIVITY;
        float y_off = yoffset * SENSITIVITY;
        if (halt == false)
            Yaw = Yaw + x_off;
        if (halt == false)
            Pitch = Pitch + y_off;

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);



    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        // TODO : fill in
        if(halt == false)
            Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // TODO : fill in


    }
};
#endif