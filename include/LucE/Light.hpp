#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <LucE/Shader.hpp>
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
	float cutoff;

	SpotLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction,  float cutoff)
		: Light(position, ambient, diffuse, specular)
	{
		this->direction = direction;
		this->cutoff = cutoff;
	}

	virtual void setUniforms(Shader &shader)
	{
		Light::setUniforms(shader);
		shader.setVec3("light.direction", this->direction);
		shader.setFloat("light.cutoff", this->cutoff);
	}
};

#endif
