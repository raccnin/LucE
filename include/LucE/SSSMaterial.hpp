#ifndef SSSMATERIAL_H
#define SSSMATERIAL_H

#include <glm/glm.hpp>
#include <LucE/Shader.hpp>

class SSSMaterial
{
public:
	glm::vec3 albedo;	

	float scattering;
	float absorption;
	float meanCosine;
	float rri;

	SSSMaterial(glm::vec3 albedo, float scattering, float absorption, float rri, float meanCosine);
	void setUniforms(Shader& shader);
};

#endif
