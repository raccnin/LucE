#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

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
};

class SpotLight: public Light
{
public:
	glm::vec3 direction;
	unsigned int cutoff;

	SpotLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction, unsigned int cutoff)
		: Light(position, ambient, diffuse, specular)
	{
		this->direction = direction;
		this->cutoff = cutoff;
	}
};

#endif
