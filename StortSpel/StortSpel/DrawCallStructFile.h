#pragma once
#include "MeshComponent.h"

struct DrawCallStruct
{
	DrawCallStruct(MeshComponent* mesh, ShaderProgramsEnum shaderEnum, int material_ID, int material_IDX, std::string name)

	{
		this->mesh = mesh;
		this->shaderEnum = shaderEnum;
		this->material_ID = material_ID;
		this->material_IDX = material_IDX;
		this->name = name;
		this->entity = nullptr;
	}
	DrawCallStruct(MeshComponent* mesh, int material_IDX, void* entity)
	{
		this->mesh = mesh;
		this->shaderEnum = mesh->getShaderProgEnum(material_IDX);
		this->material_ID = mesh->getMaterialPtr(material_IDX)->getMaterialId();
		this->material_IDX = material_IDX;
		this->name = mesh->getFilePath();
		this->entity = entity;

	}
	MeshComponent* mesh;
	ShaderProgramsEnum shaderEnum;
	int material_ID;
	int material_IDX;
	std::string name;
	void* entity;

	bool operator==(const DrawCallStruct& other)
	{
		if (this->mesh == other.mesh &&
			this->shaderEnum == other.shaderEnum &&
			this->material_ID == other.material_ID &&
			this->material_IDX == other.material_IDX &&
			this->name == other.name && this->entity == other.entity)
		{
			return true;
		}
		else
			return false;
	}
};