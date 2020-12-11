#pragma once
#include "MeshComponent.h"

struct drawCallStruct
{
	drawCallStruct(MeshComponent* mesh, ShaderProgramsEnum shaderEnum, int material_ID, int material_IDX, std::string name)
	{
		this->mesh = mesh;
		this->shaderEnum = shaderEnum;
		this->material_ID = material_ID;
		this->material_IDX = material_IDX;
		this->name = name;
	}
	MeshComponent* mesh;
	ShaderProgramsEnum shaderEnum;
	int material_ID;
	int material_IDX;
	std::string name;

	bool operator==(const drawCallStruct& other)
	{
		if (this->mesh == other.mesh &&
			this->shaderEnum == other.shaderEnum &&
			this->material_ID == other.material_ID &&
			this->material_IDX == other.material_IDX &&
			this->name == other.name)
		{
			return true;
		}
		else
			return false;
	}
};