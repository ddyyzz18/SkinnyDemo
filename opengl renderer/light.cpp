#include "light.h"


Light::Light() :
	ambient(0.0f, 0.0f, 0.0f),
	diffuse(0.0f, 0.0f, 0.0f),
	specular(0.0f, 0.0f, 0.0f),
	use(false)
{

}

Light::Light(vec3 a, vec3 d, vec3 s, bool Use)
{
	ambient = a;
	diffuse = d;
	specular = s;
	use = Use;
}

Light::~Light()
{

}

void Light::BindLight(Shader& shader)
{
	if (use)
	{
		//shader.use();
		shader.setVec3("light.ambient", ambient);
		shader.setVec3("light.diffuse", diffuse);
		shader.setVec3("light.specular", specular);
	}
}

DirLight::DirLight() :
	Light(),
	direction(0.0f, 0.0f, 0.0f)
{

}

DirLight::~DirLight()
{

}

void DirLight::BindLight(Shader& shader)
{
	if (use)
	{
		Light::BindLight(shader);
		shader.setVec3("light.direction", direction);

	}
}

PointLight::PointLight() :
	Light(),
	position(0.0f, 0.0f, 0.0f),
	constant(0.0f),
	linear(0.0f),
	quadratic(0.0f)
{

}

PointLight::~PointLight()
{

}

void PointLight::BindLight(Shader& shader)
{
	if (use)
	{
		Light::BindLight(shader);
		shader.setVec3("light.position", position);
		shader.setFloat("light.constant", constant);
		shader.setFloat("light.linear", linear);
		shader.setFloat("light.quadratic", quadratic);
	}
}

SpotLight::SpotLight() :
	Light(),
	position(0.0f, 0.0f, 0.0f),
	direction(0.0f, 0.0f, 0.0f),
	constant(0.0f),
	linear(0.0f),
	quadratic(0.0f),
	cutOff(0.0f),
	outerCutOff(0.0f)
{

}

SpotLight::~SpotLight()
{

}

void SpotLight::BindLight(Shader& shader)
{
	if (use)
	{
		Light::BindLight(shader);
		shader.setVec3("light.position", position);
		shader.setVec3("light.direction", direction);
		shader.setFloat("light.constant", constant);
		shader.setFloat("light.linear", linear);
		shader.setFloat("light.quadratic", quadratic);
		shader.setFloat("light.cutOff", cutOff);
		shader.setFloat("light.outerCutOff", outerCutOff);
	}

}