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

	Light(glm::vec3 position, glm::vec3 colour, const char* modelPath);
	virtual void setUniforms(Shader &shader);
	void draw(Shader& shader);
	virtual void setPos(glm::vec3 worldPos);
};



#endif
