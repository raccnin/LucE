#include <LucE/Spotlight.hpp>

SpotLight::SpotLight(glm::vec3 position, glm::vec3 colour, const char* modelPath, glm::vec3 target,  float innerCutoff, float outerCutoff)
	: direction(target - position), outerCutoff(outerCutoff), innerCutoff(innerCutoff), target(target), 
	camera(position, glm::vec3(0.0f, 1.0f, 0.0f), target),  
	Light(position, colour, modelPath),
	projection(glm::perspective(acos(outerCutoff)*2.0, 1.0, 0.1, 1000.0)) // outerCutoff * 2 is FOV of light
//projection(glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 7.5f))
{
}

void SpotLight::setUniforms(Shader &shader)
{
	Light::setUniforms(shader);
	shader.setVec3("light.direction", this->direction);
	shader.setFloat("light.innerCutoff", this->innerCutoff);
	shader.setFloat("light.outerCutoff", this->outerCutoff);
}

glm::mat4 SpotLight::getViewMatrix()
{
	return glm::lookAt(position, position + direction, camera.up);
}


glm::mat4 SpotLight::getTransformMatrix()
{
	return this->projection * this->camera.getViewMatrix();
}

void SpotLight::lookAt(glm::vec3 target)
{
	this->target = target;
	updateVectors();
}

void SpotLight::setPos(glm::vec3 worldPos)
{
	Light::setPos(worldPos);
	camera.setPos(worldPos);
	updateVectors();
}

void SpotLight::updateVectors()
{
	direction = glm::normalize(target - this->position);
	camera.lookAt(target);
}
