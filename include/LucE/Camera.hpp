#ifndef CAMERA_H
#define CAMERA_H

#include<glm/glm.hpp>



class Camera
{
    public:

        Camera(glm::vec3 worldPos, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 targetPos = glm::vec3(0.0f));
        glm::mat4 getViewMatrix();
        void lookAt(glm::vec3 target);
        void setPos(glm::vec3 worldPos);
        glm::vec3 up;    
        glm::vec3 worldPos;

    protected:

        glm::vec3 targetPos;
        glm::vec3 worldUp;
        glm::vec3 right;
        glm::vec3 front;
        void updateVectors();
};


class OrbitalCamera: public Camera
{

    public:
    
        OrbitalCamera(float radius, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 targetPos = glm::vec3(0.0f));
        void processMouseMovement(float xoffset, float yoffset);
    private:

        float radius;
        float yaw;
        float pitch;
        float sensitivity;
        void updateWorldPos();

};

#endif