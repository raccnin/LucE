#include <LucE/Light.hpp>


Light::Light(glm::vec3 position, glm::vec3 colour, const char* modelPath)
	: position(position), 
		ambient(colour * glm::vec3(0.1)),
		diffuse(colour * glm::vec3(0.5)),
		specular(colour),
		model(modelPath)
	{
		model.setPos(position);
	}

void Light::setUniforms(Shader &shader)
{
	//shader.use();
	shader.setVec3("light.position", this->position);
	shader.setVec3("light.ambient", this->ambient);
	shader.setVec3("light.diffuse", this->diffuse);
	shader.setVec3("light.specular", this->specular);
	//glUseProgram(0);
}

void Light::draw(Shader& shader)
{
	this->model.draw(shader);
}

void Light::setPos(glm::vec3 worldPos)
{
	model.setPos(worldPos);
	position = worldPos;
}
