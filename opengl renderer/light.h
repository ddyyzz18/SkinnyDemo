#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>
#include "shader.h"

using namespace glm;

class Light
{

public:
	Light();
	Light(vec3, vec3, vec3,bool);
	void BindLight(Shader& shader);
	~Light();
public:
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	bool use;						//使ってるかどうか
	
};

//平行光
class DirLight:public Light
{
public:
	DirLight();
	~DirLight();
	void BindLight(Shader& shader);
public:
	vec3 direction;

};

//点光源
class PointLight :public Light
{
public:
	PointLight();
	~PointLight();
	void BindLight(Shader& shader);
public:
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

//スポットライト
class SpotLight :public Light
{
public:
	SpotLight();
	~SpotLight();
	void BindLight(Shader& shader);
public:
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

#endif