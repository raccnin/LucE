#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include <LucE/Light.hpp>

class SpotLight: public Light
{
public:
	glm::vec3 direction;
	float outerCutoff;
	float innerCutoff;

	SpotLight(glm::vec3 position, glm::vec3 colour, const char* modelPath, glm::vec3 target,  float innerCutoff, float outerCutoff);

	virtual void setUniforms(Shader &shader);
	glm::mat4 getViewMatrix();
	glm::mat4 getTransformMatrix();
	void lookAt(glm::vec3 target);
	virtual void setPos(glm::vec3 worldPos);

private:
	void updateVectors();
	glm::vec3 target;
	Camera camera;
	glm::mat4 projection;

};

#endif
