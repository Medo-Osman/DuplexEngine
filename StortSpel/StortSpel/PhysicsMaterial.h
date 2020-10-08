#pragma once
#include"3DPCH.h"



struct PhysicsMaterial
{
	std::string name;
	float staticFriction;
	float dynamicFriction;
	float restitution;
	physx::PxCombineMode::Enum frictionCombineMode;
	physx::PxCombineMode::Enum restitutionCombineMode;
	PhysicsMaterial(std::string name, float staticFriction, float dynamicFriction, float restitution, physx::PxCombineMode::Enum frictionCombineMode, physx::PxCombineMode::Enum restitutionCombineMode)
	{
		this->name = name;
		this->staticFriction = staticFriction;
		this->dynamicFriction = dynamicFriction;
		this->restitution = restitution;
		this->frictionCombineMode = frictionCombineMode;
		this->restitutionCombineMode = restitutionCombineMode;
	}

	static void createDefaultMaterials(std::vector<PhysicsMaterial> &vec)
	{
		vec.emplace_back("default", 0.5f, 0.5f, 0.0f, physx::PxCombineMode::eMIN, physx::PxCombineMode::eMAX);
		vec.emplace_back("ice", 0.f, 0.f, 0.0f, physx::PxCombineMode::eMIN, physx::PxCombineMode::eMAX);
		vec.emplace_back("wood", 0.7f, 0.7f, 0.05f, physx::PxCombineMode::eMIN, physx::PxCombineMode::eMAX);
		vec.emplace_back("earth", 0.5f, 0.5f, 0.05f, physx::PxCombineMode::eMIN, physx::PxCombineMode::eMAX);
		vec.emplace_back("human", 1.f, 1.f, 0.f, physx::PxCombineMode::eAVERAGE, physx::PxCombineMode::eMAX);
	}

};

