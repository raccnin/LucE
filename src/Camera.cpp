#include <LucE/Camera.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// public
// ------

Camera::Camera(glm::vec3 worldPos, glm::vec3 worldUp, glm::vec3 targetPos /* default glm::vec3(1.0f) */)
	: worldPos(worldPos), worldUp(glm::normalize(worldUp)), targetPos(targetPos), speed(10.0f), yaw(-90.0f), pitch(0.0f), sensitivity(0.1f)
{
    updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(worldPos, worldPos + front, up);
}

void Camera::lookAt(glm::vec3 targetPos)
{
		glm::vec3 direction = normalize(targetPos - worldPos);
    updateVectors();
}

void Camera::setPos(glm::vec3 worldPos)
{
    this->worldPos = worldPos;
    updateVectors();
}

void Camera::processKeyboard(Camera_Direction dir, float deltaTime)
{
	float velocity = speed * deltaTime;
	switch (dir)
	{
		case FORWARD:
			worldPos += front * velocity;
			break;
		case BACKWARD:
			worldPos -= front * velocity;
			break;
		case LEFT:
			worldPos -= right * velocity;
			break;
		case RIGHT:
			worldPos += right * velocity;
			break;
	}
	updateVectors();
}

// private
// -------

void Camera::updateVectors()
{
	glm::vec3 newFront;		
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(newFront);	
	right = glm::cross(front,worldUp);
	up = glm::cross(right, front);
}

void Camera::processMouse(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}

	updateVectors();
}
