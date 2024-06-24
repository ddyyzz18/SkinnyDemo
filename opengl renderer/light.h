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
	bool use;						//�g���Ă邩�ǂ���
	
};

//���s��
class DirLight:public Light
{
public:
	DirLight();
	~DirLight();
	void BindLight(Shader& shader);
public:
	vec3 direction;

};

//�_����
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

//�X�|�b�g���C�g
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