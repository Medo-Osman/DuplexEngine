#pragma once
#include"3DPCH.h"
#include"Entity.h"
#include"MeshComponent.h"
#include"BoundingVolumeHolder.h"
#include"OptimizationConfiguration.h"
#include"DrawCallStructFile.h"


class QuadTree
{

private:
	const static int NR_OF_VERTICIES_FOR_PARTITION = 500;
	Vector3 m_maxPos;
	Vector3 m_minPos;
	const bool addBoundingBoxes = false;

	std::vector<DrawCallStruct*> m_drawCallStructs;


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


	void partition(const XMFLOAT3& minPos, const XMFLOAT3& maxPos, const std::vector<DrawCallStruct*>& drawCallStructVector)
	{
		if (!USE_QUADTREE) return;
		int totalNrOfVerticies = 0;

		for (int i = 0; i < (int)drawCallStructVector.size(); i++)
		{

			std::vector<Component*>  meshCompVec;
			static_cast<Entity*>(drawCallStructVector.at(i)->entity)->getComponentsOfType(meshCompVec, ComponentType::MESH);


			for (int j = 0; j < meshCompVec.size(); j++)
			{

				totalNrOfVerticies += dynamic_cast<MeshComponent*>(meshCompVec.at(j))->getMeshResourcePtr()->getVertexBuffer().getSize();
			}
		}


		if (totalNrOfVerticies >= NR_OF_VERTICIES_FOR_PARTITION && drawCallStructVector.size() > 4) //Partition, since we do not split models make sure there is more than one model
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

			std::vector<DrawCallStruct*> topLeftObj;
			std::vector<DrawCallStruct*> topRightObj;
			std::vector<DrawCallStruct*> botLeftObj;
			std::vector<DrawCallStruct*> botRightObj;
			std::vector<DrawCallStruct*> center;


			for (int i = 0; i < (int)drawCallStructVector.size(); i++)
			{
				Entity* entity = static_cast<Entity*>(drawCallStructVector.at(i)->entity);
				Vector3 min = { 9999, 9999, 9999 }, max = { -9999, -9999, -9999 };
				BoundingOrientedBox bb;
				getMinMaxFromEntity(entity, min, max);
				bb.Center = entity->getTranslation();
				bb.Extents = (max - min) * 0.5f;
				bb.Orientation = entity->getRotation();

				if (this->contains(bb, minPos, newCenterMaxY)) //For BottomLeft
				{
					botLeftObj.emplace_back(drawCallStructVector.at(i));
				}
				else if (this->contains(bb, newCenterMinY, maxPos)) //For TopRight
				{
					topRightObj.emplace_back(drawCallStructVector.at(i));
				}
				else if (this->contains(bb, botRightMin, botRightMax)) //For botRight
				{
					botRightObj.emplace_back(drawCallStructVector.at(i));
				}
				else if (this->contains(bb, topLeftMin, topLeftMax)) //For TopLeft
				{
					topLeftObj.emplace_back(drawCallStructVector.at(i));
				}
				else
				{
					center.emplace_back(drawCallStructVector.at(i));
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
					this->m_drawCallStructs.emplace_back(center.at(i));
				}
				this->m_drawCallStructs = center;
			}

		}
		else
		{
			for (int i = 0; i < (int)m_drawCallStructs.size(); i++)
			{
				this->m_drawCallStructs.emplace_back(m_drawCallStructs.at(i));
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

	QuadTree(const XMFLOAT3& minPos, const XMFLOAT3& maxPos, const std::vector<DrawCallStruct*>& drawCallStructVector) //Used when creating new quadtrees inside datastructure. Assumes correct min/max pos was sent for Entitys.

	{
		this->m_maxPos = maxPos;
		this->m_minPos = minPos;
		this->m_topLeftQuad = nullptr;
		this->m_topRightQuad = nullptr;
		this->m_botLeftQuad = nullptr;
		this->m_botRightQuad = nullptr;

		this->partition(minPos, maxPos, drawCallStructVector);
	}

	void getRenderList(const BoundingFrustum* frust, std::vector<DrawCallStruct*> &drawCallStructVectorOut, std::vector<BoundingVolumeHolder>& vecOut)
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
			if (m_drawCallStructs.size() >= 1)
			{
				for (int i = 0; i < (int)m_drawCallStructs.size(); i++)
				{
					bool draw = true;
					MeshComponent* meshComponent;
					Entity* parentEntity = static_cast<Entity*>(m_drawCallStructs.at(i)->entity);
					std::vector<Component*> compVect;
					parentEntity->getComponentsOfType(compVect, ComponentType::MESH);

					for (size_t j = 0; j < compVect.size(); j++)
					{
						meshComponent = static_cast<MeshComponent*>(compVect.at(j));

						if (parentEntity->m_canCull && USE_FRUSTUM_CULLING)
						{
							//Culling
							Vector3 scale = meshComponent->getScaling() * parentEntity->getScaling();
							XMVECTOR pos = parentEntity->getTranslation();
							pos += meshComponent->getTranslation();
							pos += meshComponent->getMeshResourcePtr()->getBoundsCenter() * scale;
							XMFLOAT3 posFloat3;
							XMStoreFloat3(&posFloat3, pos);

							if (frust->Contains(pos) != ContainmentType::CONTAINS)
							{
								meshComponent->getMeshResourcePtr()->getMinMax(min, max);
								XMFLOAT3 ext = (max - min) / 2;
								ext = ext * scale;
								XMFLOAT4 rot = parentEntity->getRotation() * meshComponent->getRotation();
								BoundingOrientedBox box(posFloat3, ext, rot);

								ContainmentType contType = frust->Contains(box);
								draw = (contType == ContainmentType::INTERSECTS || contType == ContainmentType::CONTAINS);
							}
							else
							{
								draw = true;
							}
						}
						if(draw)
							drawCallStructVectorOut.emplace_back(m_drawCallStructs.at(i));
					}
				}
			}
			if (this->m_topLeftQuad != nullptr)
				this->m_topLeftQuad->getRenderList(frust, drawCallStructVectorOut, vecOut);
			if (this->m_topRightQuad != nullptr)
				this->m_topRightQuad->getRenderList(frust, drawCallStructVectorOut, vecOut);
			if (this->m_botLeftQuad != nullptr)
				this->m_botLeftQuad->getRenderList(frust, drawCallStructVectorOut, vecOut);
			if (this->m_botRightQuad != nullptr)
				this->m_botRightQuad->getRenderList(frust, drawCallStructVectorOut, vecOut);
		}

	}


	void partition(std::unordered_map<std::string, Entity*>& entityMap)
	{
		if (!USE_QUADTREE) return;

		//std::vector<Entity*> tempEntityVec; 
		std::vector<DrawCallStruct*> tempDrawCallStructVector;
		Vector3 min = XMVectorSet(99999, 99999, 99999, 1), max = XMVectorSet(-99999, -99999, -99999, 1);
		//Calculate min/max and set them
		for (auto& entityItterator : entityMap) {
			Entity* entity = entityItterator.second;


			PhysicsComponent* physComp = dynamic_cast<PhysicsComponent*>(entity->getComponent("physics"));
			if (physComp && physComp->isStatic()) //We only wanna keep static s****.
			{
			
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

							for (size_t j = 0; j < meshResource->getMaterialCount(); j++)
							{
								tempDrawCallStructVector.emplace_back(new DrawCallStruct(meshComponent, j, entity));

							}
							
						}
					}
				}
			}

		}

		m_maxPos = max;
		m_minPos = min;
		partition(this->m_minPos, this->m_maxPos, tempDrawCallStructVector);

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
		if (m_drawCallStructs.size() >= 1)
		{
			for (int i = 0; i < (int)m_drawCallStructs.size(); i++)
			{
				vecOut.emplace_back(m_drawCallStructs.at(i)->mesh);
				//std::vector<Component*> compVect;
				//m_drawCallStructs.at(i)->entity->getComponentsOfType(compVect, ComponentType::MESH);
				//for (size_t j = 0; j < compVect.size(); j++)
				//{
				//	vecOut.emplace_back(static_cast<MeshComponent*>(compVect.at(j)));
				//}
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