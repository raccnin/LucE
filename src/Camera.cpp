#include <LucE/Camera.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// public
// ------

Camera::Camera(glm::vec3 worldPos, glm::vec3 worldUp, glm::vec3 targetPos /* default glm::vec3(1.0f) */)
{
    this->worldPos = worldPos;
    this->worldUp = glm::normalize(worldUp);
    this->targetPos = targetPos;
    updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(worldPos, targetPos, up);
}

void Camera::lookAt(glm::vec3 targetPos)
{
    this->targetPos = targetPos;
    updateVectors();
}

void Camera::setPos(glm::vec3 worldPos)
{
    this->worldPos = worldPos;
    updateVectors();
}

// private
// -------

void Camera::updateVectors()
{
    front = glm::normalize(worldPos - targetPos);
    right = glm::cross(worldUp, front);
    up = glm::cross(front, right);
}

// Orbital Camera
// --------------

// public
OrbitalCamera::OrbitalCamera(float radius, glm::vec3 worldUp /* default positive y */, glm::vec3 targetPos /* default origin */):
    Camera(glm::vec3(targetPos.x, targetPos.y, targetPos.z + radius), worldUp, targetPos) // construct as per normal but with starting position at radius away
{
    this->radius = radius;
    yaw = -90.0f;
    pitch = 0.0f;
    sensitivity = 1.0f;
}

void OrbitalCamera::processMouseMovement(float xoffset, float yoffset)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    updateWorldPos();
    updateVectors();
}

// private

void OrbitalCamera::updateWorldPos()
{
    glm::vec3 toTarget = worldPos - targetPos; // camera to target relative to origin
    glm::mat4 camTransform = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), up); // rotate yaw around up vector
    camTransform = glm::rotate(camTransform, glm::radians(pitch), right); // rotate pitch around right vector
    camTransform = glm::translate(camTransform, targetPos); // bring back position relative to target
    worldPos = glm::vec3(camTransform * glm::vec4(worldPos, 1.0f));
}
