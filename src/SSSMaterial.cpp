#include <LucE/SSSMaterial.hpp>


SSSMaterial::SSSMaterial(glm::vec3 albedo, float scattering, float absorption, float rri, float meanCosine)
: scattering(scattering), absorption(absorption),  meanCosine(meanCosine), rri(rri), albedo(albedo)
{
}

void SSSMaterial::setUniforms(Shader& shader)
{
	shader.setVec3("material.albedo", albedo);

	shader.setFloat("material.scattering", scattering);
	shader.setFloat("material.absorption", absorption);
	shader.setFloat("material.meanCosine", meanCosine);
	shader.setFloat("material.rri", rri);
}
