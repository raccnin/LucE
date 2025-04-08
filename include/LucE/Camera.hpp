#ifndef CAMERA_H
#define CAMERA_H

#include<glm/glm.hpp>

enum Camera_Direction
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	glm::vec3 up;    
	glm::vec3 right;
	glm::vec3 worldPos;

	Camera(glm::vec3 worldPos, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 targetPos = glm::vec3(0.0f));
	glm::mat4 getViewMatrix();
	void lookAt(glm::vec3 target);
	void lookAt(float x, float y, float z);
	void setPos(glm::vec3 worldPos);
	glm::vec3 getTarget(){
		return this->targetPos;
	}
	void processKeyboard(Camera_Direction dir, float deltaTime);
	void processMouse(float xoffset, float yoffset , bool constrainPitch = true);

protected:
	glm::vec3 targetPos;
	glm::vec3 worldUp;
	glm::vec3 front;
	float yaw;
	float pitch;
	float speed;
	float sensitivity;

	void updateVectors();
};
#endif
