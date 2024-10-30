#include <LucE/Camera.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

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