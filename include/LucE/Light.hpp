#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <LucE/Shader.hpp>
#include <LucE/Camera.hpp>
#include <glad.h>
#include <iostream>

class Light
{
public:
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
	{
		this->position = position;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
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
};

class SpotLight: public Light
{
public:
	glm::vec3 direction;
	float outerCutoff;
	float innerCutoff;

	SpotLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction,  float innerCutoff, float outerCutoff)
		: direction(direction), outerCutoff(outerCutoff), innerCutoff(innerCutoff), 
			camera(position, glm::vec3(0.0f, 1.0f, 0.0f), position + direction),  // direction of camera different from direction of spotlight
			Light(position, ambient, diffuse, specular)
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
private:
	Camera camera;
};

#endif
