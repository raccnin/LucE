#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <LucE/Shader.hpp>
#include <LucE/Camera.hpp>
#include <LucE/Model.hpp>
#include <glad.h>

class Light
{
public:
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Model model;

	Light(glm::vec3 position, glm::vec3 colour, const char* modelPath)
	: position(position), 
		ambient(colour * glm::vec3(0.1)),
		diffuse(colour * glm::vec3(0.5)),
		specular(colour),
		model(modelPath)
	{
		model.setPos(position);
	}

	virtual void setUniforms(Shader &shader)
	{
		//shader.use();
		shader.setVec3("light.position", this->position);
		shader.setVec3("light.ambient", this->ambient);
		shader.setVec3("light.diffuse", this->diffuse);
		shader.setVec3("light.specular", this->specular);
		//glUseProgram(0);
	}

	void draw(Shader& shader)
	{
		this->model.draw(shader);
	}

	virtual void setPos(glm::vec3 worldPos)
	{
		model.setPos(worldPos);
		position = worldPos;
	}
};

class SpotLight: public Light
{
public:
	glm::vec3 direction;
	float outerCutoff;
	float innerCutoff;

	SpotLight(glm::vec3 position, glm::vec3 colour, const char* modelPath, glm::vec3 target,  float innerCutoff, float outerCutoff)
		: direction(target - position), outerCutoff(outerCutoff), innerCutoff(innerCutoff), target(target), 
			camera(position, glm::vec3(0.0f, 1.0f, 0.0f), target),  
			Light(position, colour, modelPath),
			projection(glm::perspective(acos(outerCutoff)*2.0, 1.0, 0.1, 1000.0)) // outerCutoff * 2 is FOV of light
			//projection(glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 7.5f))
	{
	}

virtual void setUniforms(Shader &shader)
	{
		Light::setUniforms(shader);
		shader.setVec3("light.direction", this->direction);
		shader.setFloat("light.innerCutoff", this->innerCutoff);
		shader.setFloat("light.outerCutoff", this->outerCutoff);
	}

	glm::mat4 getViewMatrix()
	{
		return this->camera.getViewMatrix();
	}

	glm::mat4 getTransformMatrix()
	{
		return this->projection * this->camera.getViewMatrix();
	}

	void lookAt(glm::vec3 target)
	{
		this->target = target;
		updateVectors();
	}

	virtual void setPos(glm::vec3 worldPos)
	{
		Light::setPos(worldPos);
		camera.setPos(worldPos);
		updateVectors();
	}

private:
	glm::vec3 target;
	Camera camera;
	glm::mat4 projection;

	void updateVectors()
	{
		direction = target - this->position;
		camera.lookAt(target);
	}
};

#endif
