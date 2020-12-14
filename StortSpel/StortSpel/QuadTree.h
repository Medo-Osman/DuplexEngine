#pragma once
#include"3DPCH.h"
#include"Entity.h"
#include"MeshComponent.h"
#include"BoundingVolumeHolder.h"
#include"OptimizationConfiguration.h"

class QuadTree
{

private:
	const static int NR_OF_VERTICIES_FOR_PARTITION = 500;
	Vector3 m_maxPos;
	Vector3 m_minPos;
	const bool addBoundingBoxes = false;

	std::vector<Entity*> m_entities;

	QuadTree* m_topLeftQuad;
	QuadTree* m_topRightQuad;
	QuadTree* m_botLeftQuad;
	QuadTree* m_botRightQuad;
	bool contains(BoundingOrientedBox bb, Vector3 min, Vector3 max)
	{
		bool isInsideQuad = false;
		BoundingBox bigBoundingB;
		bigBoundingB.Center = (min + max) * 0.5f;
		bigBoundingB.Extents = (max - min) * 0.5f;
		if(bigBoundingB.Contains(bb) == ContainmentType::CONTAINS )
		//if (center.x > min.x && center.x < max.x
		//	&& center.y > min.y && center.y < max.y
		//	&& center.z > min.z && center.z < max.z)
		{
			isInsideQuad = true;
		}

		return isInsideQuad;
	}


	void partition(const XMFLOAT3& minPos, const XMFLOAT3& maxPos, const std::vector<Entity*>& entitys)
	{
		if (!USE_QUADTREE) return;
		int totalNrOfVerticies = 0;

		for (int i = 0; i < (int)entitys.size(); i++)
		{

			std::vector<Component*>  meshCompVec;
			entitys.at(i)->getComponentsOfType(meshCompVec, ComponentType::MESH);

			for (int j = 0; j < meshCompVec.size(); j++)
			{

				totalNrOfVerticies += dynamic_cast<MeshComponent*>(meshCompVec.at(j))->getMeshResourcePtr()->getVertexBuffer().getSize();
			}
		}

		if (totalNrOfVerticies >= NR_OF_VERTICIES_FOR_PARTITION && entitys.size() > 1) //Partition, since we do not split models make sure there is more than one model
		{
			//Calculate new center
			XMFLOAT3 newCenter = { (minPos.x + maxPos.x) / 2, (minPos.y + maxPos.z) / 2, (minPos.z + maxPos.z) / 2 };
			XMFLOAT3 newCenterMinY = newCenter, newCenterMaxY = newCenter;
			float centerToBotZ = newCenter.z - minPos.z;
			XMFLOAT3 botRightMin = XMFLOAT3(newCenter.x, minPos.y, newCenter.z - centerToBotZ);
			XMFLOAT3 botRightMax = XMFLOAT3(maxPos.x, maxPos.y, maxPos.z - centerToBotZ);
			XMFLOAT3 topLeftMin = XMFLOAT3(minPos.x, minPos.y, minPos.z + centerToBotZ);
			XMFLOAT3 topLeftMax = XMFLOAT3(newCenter.x, maxPos.y, newCenter.z + centerToBotZ);
			newCenterMaxY.y = maxPos.y, newCenterMinY.y = minPos.y;

			std::vector<Entity*> topLeftObj;
			std::vector<Entity*> topRightObj;
			std::vector<Entity*> botLeftObj;
			std::vector<Entity*> botRightObj;
			std::vector<Entity*> center;


			for (int i = 0; i < (int)entitys.size(); i++)
			{
				Vector3 min = { 9999, 9999, 9999 }, max = { -9999, -9999, -9999 };
				BoundingOrientedBox bb;
				getMinMaxFromEntity(entitys.at(i), min, max);
				bb.Center = entitys[i]->getTranslation();
				bb.Extents = (max - min) * 0.5f;
				bb.Orientation = entitys.at(i)->getRotation();

				if (this->contains(bb, minPos, newCenterMaxY)) //For BottomLeft
				{
					botLeftObj.emplace_back(entitys.at(i));
				}
				else if (this->contains(bb, newCenterMinY, maxPos)) //For TopRight
				{
					topRightObj.emplace_back(entitys.at(i));
				}
				else if (this->contains(bb, botRightMin, botRightMax)) //For botRight
				{
					botRightObj.emplace_back(entitys.at(i));
				}
				else if (this->contains(bb, topLeftMin, topLeftMax)) //For TopLeft
				{
					topLeftObj.emplace_back(entitys.at(i));
				}
				else
				{
					center.emplace_back(entitys.at(i));
				}
			}

			if (botLeftObj.size() >= 1)
			{
				m_botLeftQuad = new QuadTree(minPos, newCenterMaxY, botLeftObj);
			}

			if (topRightObj.size() >= 1)
			{
				m_topRightQuad = new QuadTree(newCenterMinY, maxPos, topRightObj);
			}

			if (botRightObj.size() >= 1)
			{
				m_botRightQuad = new QuadTree(botRightMin, botRightMax, botRightObj);
			}

			if (topLeftObj.size() >= 1)
			{
				m_topLeftQuad = new QuadTree(topLeftMin, topLeftMax, topLeftObj);
			}

			if (center.size() >= 1)
			{
				for (int i = 0; i < center.size(); i++)
				{
					this->m_entities.emplace_back(center.at(i));
				}
				this->m_entities = center;
			}

		}
		else
		{
			for (int i = 0; i < (int)entitys.size(); i++)
			{
				this->m_entities.emplace_back(entitys.at(i));
			}
		}
	}

public:
	QuadTree()
	{
		this->m_maxPos = { 0.f, 0.f, 0.f };
		this->m_minPos = { 0.f, 0.f, 0.f };
		this->m_topLeftQuad = nullptr;
		this->m_topRightQuad = nullptr;
		this->m_botLeftQuad = nullptr;
		this->m_botRightQuad = nullptr;
	}
	QuadTree(const XMFLOAT3& minPos, const XMFLOAT3& maxPos, const std::vector<Entity*>& Entitys) //Used when creating new quadtrees inside datastructure. Assumes correct min/max pos was sent for Entitys.
	{
		this->m_maxPos = maxPos;
		this->m_minPos = minPos;
		this->m_topLeftQuad = nullptr;
		this->m_topRightQuad = nullptr;
		this->m_botLeftQuad = nullptr;
		this->m_botRightQuad = nullptr;

		this->partition(minPos, maxPos, Entitys);

	}

	void getRenderList(const BoundingFrustum* frust, std::vector<MeshComponent*> & meshComponentVec, std::vector<BoundingVolumeHolder>& vecOut)
	{

		Vector3 min = m_minPos, max = m_maxPos, center, ext;
		center = (min + max) * 0.5f;
		ext = (max - min) * 0.5f;

		DirectX::BoundingBox bb;
		XMStoreFloat3(&bb.Center, center);
		XMStoreFloat3(&bb.Extents, ext);

		if(addBoundingBoxes)
			vecOut.emplace_back(BoundingVolumeTypes::BOX, new BoundingBox(bb));

		if (frust->Contains(bb) == ContainmentType::INTERSECTS || frust->Contains(bb) == ContainmentType::CONTAINS)
		{
			if (m_entities.size() >= 1)
			{
				for (int i = 0; i < (int)m_entities.size(); i++)
				{
					std::vector<Component*> compVect;
					m_entities.at(i)->getComponentsOfType(compVect, ComponentType::MESH);
					for (size_t j = 0; j < compVect.size(); j++)
					{
						meshComponentVec.emplace_back(static_cast<MeshComponent*>(compVect.at(j)));
					}
				}
			}
			if (this->m_topLeftQuad != nullptr)
				this->m_topLeftQuad->getRenderList(frust, meshComponentVec, vecOut);
			if (this->m_topRightQuad != nullptr)
				this->m_topRightQuad->getRenderList(frust, meshComponentVec, vecOut);
			if (this->m_botLeftQuad != nullptr)
				this->m_botLeftQuad->getRenderList(frust, meshComponentVec, vecOut);
			if (this->m_botRightQuad != nullptr)
				this->m_botRightQuad->getRenderList(frust, meshComponentVec, vecOut);
		}

	}
	void partition(std::unordered_map<std::string, Entity*>& entityMap)
	{
		std::vector<Entity*> tempEntityVec; 
		Vector3 min = XMVectorSet(99999, 99999, 99999, 1), max = XMVectorSet(-99999, -99999, -99999, 1);
		//Calculate min/max and set them
		for (auto& entityItterator : entityMap) {
			Entity* entity = entityItterator.second;

			PhysicsComponent* physComp = dynamic_cast<PhysicsComponent*>(entity->getComponent("physics"));
			if (physComp && physComp->isStatic()) //We only wanna keep static s****.
			{
			
				std::vector<Component*>  meshCompVec;
				entity->getComponentsOfType(meshCompVec, ComponentType::MESH);

				bool added = false;
				for (int i = 0; i < meshCompVec.size(); i++)
				{
					MeshComponent* meshComponent = static_cast<MeshComponent*>(meshCompVec.at(i));
					MeshResource* meshResource = meshComponent->getMeshResourcePtr();
					if (meshResource)
					{
						PositionVertex* verts = meshResource->getVertexArray();
						if (verts)
						{
							for (int y = 0; y < (int)meshResource->getVertexBuffer().getSize(); y++)
							{
								Vector3 pos = verts[y].position;
								pos = XMVector3Transform(pos, XMMatrixIdentity() * XMMatrixScalingFromVector(entity->getScaling())
									* XMMatrixRotationQuaternion(entity->getRotation())
									* XMMatrixTranslationFromVector(entity->getTranslation()));
								min = XMVectorMin(min, pos);
								max = XMVectorMax(max, pos);
							}
							if(!added)
							{
								tempEntityVec.emplace_back(entity);
							}
						}
					}
				}
			}

		}

		m_maxPos = max;
		m_minPos = min;
		partition(this->m_minPos, this->m_maxPos, tempEntityVec);

	}

	void getMinMaxFromEntity(Entity* entity, Vector3 &min, Vector3 &max)
	{
		PhysicsComponent* physComp = dynamic_cast<PhysicsComponent*>(entity->getComponent("physics"));

		std::vector<Component*>  meshCompVec;
		entity->getComponentsOfType(meshCompVec, ComponentType::MESH);

		for (int i = 0; i < meshCompVec.size(); i++)
		{
			MeshComponent* meshComponent = static_cast<MeshComponent*>(meshCompVec.at(i));
			MeshResource* meshResource = meshComponent->getMeshResourcePtr();
			if (meshResource)
			{
				PositionVertex* verts = meshResource->getVertexArray();
				if (verts)
				{
					for (int y = 0; y < (int)meshResource->getVertexBuffer().getSize(); y++)
					{
						Vector3 pos = verts[y].position;
						pos = XMVector3Transform(pos, XMMatrixIdentity() * XMMatrixScalingFromVector(entity->getScaling())
							* XMMatrixRotationQuaternion(entity->getRotation())
							* XMMatrixTranslationFromVector(entity->getTranslation()));
						min = XMVectorMin(min, pos);
						max = XMVectorMax(max, pos);
					}
				}
			}
		}
	}

	const XMFLOAT3& getMinPos()
	{
		return this->m_minPos;
	}

	const XMFLOAT3& getMaxPos()
	{
		return this->m_maxPos;
	}

	void getAllMeshComponents(std::vector<MeshComponent*> &vecOut)
	{
		if (m_entities.size() >= 1)
		{
			for (int i = 0; i < (int)m_entities.size(); i++)
			{
				std::vector<Component*> compVect;
				m_entities.at(i)->getComponentsOfType(compVect, ComponentType::MESH);
				for (size_t j = 0; j < compVect.size(); j++)
				{
					vecOut.emplace_back(static_cast<MeshComponent*>(compVect.at(j)));
				}
			}
		}
		if (this->m_topLeftQuad != nullptr)
			this->m_topLeftQuad->getAllMeshComponents(vecOut);
		if (this->m_topRightQuad != nullptr)
			this->m_topRightQuad->getAllMeshComponents(vecOut);
		if (this->m_botLeftQuad != nullptr)
			this->m_botLeftQuad->getAllMeshComponents(vecOut);
		if (this->m_botRightQuad != nullptr)
			this->m_botRightQuad->getAllMeshComponents(vecOut);

	}

};