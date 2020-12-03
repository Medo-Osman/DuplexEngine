#pragma once
#include"3DPCH.h"
#include"Component.h"
#include"Physics.h"
#include"MeshComponent.h"


class PhysicsComponent : public Component
{
private:
	Physics* m_physicsPtr;
	PxRigidActor* m_actor;
	Transform* m_transform;
	Vector3 m_posOffset;
	PxShape* m_shape;
	std::vector<PxShape*> m_shapes;
	bool m_dynamic;
	bool m_controllRotation;
	bool m_kinematic;
	bool m_slide;
	bool m_hasMirrored;
	bool m_hasEntRot;

	Vector3 m_scale = {1.f, 1.f, 1.f}; 
	Vector3 m_centerOffset = {0.f, 0.f, 0.f};
	Vector3 m_meshOffset = {0.f, 0.f, 0.f};

	physx::PxGeometry* createPrimitiveGeometry(physx::PxGeometryType::Enum geometryType, XMFLOAT3 min, XMFLOAT3 max, MeshResource* meshResource)
	{
		PxTriangleMesh* tringMesh;
		PositionVertex* vertexArray;
		PxGeometry* createdGeometry = nullptr;
		XMFLOAT3 vec = XMFLOAT3((max.x - min.x) / 2, (max.y - min.y) / 2, (max.z - min.z) / 2);
		switch (geometryType)
		{
		case physx::PxGeometryType::ePLANE:
			createdGeometry = new PxPlaneGeometry();
		case physx::PxGeometryType::eCAPSULE:
			createdGeometry = new PxCapsuleGeometry(vec.x, vec.y);
			break;
		case physx::PxGeometryType::eSPHERE:
			XMFLOAT3 center = { (max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f };
			float radius;
			radius = 0;
			vertexArray = meshResource->getVertexArray();

			for (int i = 0; i < meshResource->getVertexArraySize(); i++)
			{
				XMFLOAT3 position = vertexArray[i].position;
				float tempDist = sqrt((position.x - center.x) * (position.x - center.x)
					+ (position.y - center.y) * (position.y - center.y)
					+ (position.z - center.z) * (position.z - center.z));
				if (tempDist > radius)
					radius = tempDist;
			}

			createdGeometry = new physx::PxSphereGeometry(radius);
			break;
		case physx::PxGeometryType::eBOX:
			if (max.y == min.y)
				max.y += 0.1f;
			if (max.x == min.x)
				max.x += 0.1f;
			if (max.z == min.z)
				max.z += 0.1f;

			createdGeometry = new physx::PxBoxGeometry((max.x - min.x) / 2, (max.y - min.y) / 2, (max.z - min.z) / 2);
			break;
		case physx::PxGeometryType::eTRIANGLEMESH:
			tringMesh = m_physicsPtr->getTriangleMeshe(meshResource->getFilePath(), meshResource->getVertexArraySize(), meshResource->getVertexArray(), meshResource->getIndexArraySize(), meshResource->getIndexArray(), m_centerOffset);
			createdGeometry = new physx::PxTriangleMeshGeometry(tringMesh, PxMeshScale(PxVec3(m_scale.x, m_scale.y, m_scale.z)), PxMeshGeometryFlag::eDOUBLE_SIDED);
			break;
		default:
			break;
		}

		return createdGeometry;
	}


	bool canAddGeometry(bool needsToBeStatic = false)
	{
		ErrorLogger* e = &ErrorLogger::get();
		if (!m_actor)
		{
			e->logError("No actor created before trying to add geometry!");
			return false;
		}

		if (m_shape)
		{
			e->logError("Shape already exists for actor.");
			return false;
		}

		if (needsToBeStatic)
		{
			if (m_dynamic)
			{
				e->logError("Actor is dynamic while geometry needs it to be static.");
				return false;
			}
		}

		return true;
	}
	
public:
	PhysicsComponent()
	{
		m_type = ComponentType::PHYSICS;
		m_physicsPtr = &Physics::get();
		m_dynamic = false;
		m_controllRotation = true;
		m_kinematic = false;
		m_slide = false;
	}
	~PhysicsComponent()
	{
		if(m_actor)
			m_physicsPtr->removeActor(m_actor);

		m_physicsPtr = nullptr;
		m_actor = nullptr;
		m_transform = nullptr;
		m_shape = nullptr;
	}

	void initActorAndShape(int sceneID, Entity* entity, const MeshComponent* meshComponent, PxGeometryType::Enum geometryType, bool dynamic = false, std::string physicsMaterialName = "default", bool unique = false)
	{
		bool loaded = false;
		bool forceMakeKinematic = geometryType == PxGeometryType::eTRIANGLEMESH && dynamic;
		m_dynamic = dynamic;
		m_transform = entity;
		XMFLOAT3 scale = entity->getScaling() * meshComponent->getScaling();
		m_scale = scale;
		std::string name = meshComponent->getFilePath() + std::to_string(geometryType);
		PxGeometry* geometry; 
		XMFLOAT3 boundsCenter;
		m_meshOffset = meshComponent->getTranslation();
		if (geometryType == PxGeometryType::eTRIANGLEMESH)
		{
			boundsCenter = { 0, 0, 0 };
		}
		else
		{
			meshComponent->getMeshResourcePtr()->getBoundsCenter(boundsCenter);
			boundsCenter = boundsCenter * scale;
		}
		Vector3 meshTranslation = meshComponent->getTranslation();

		m_hasEntRot = !XMQuaternionIsIdentity(entity->getRotation());

		if(m_hasEntRot)
			meshTranslation = XMVector3Rotate(meshTranslation, entity->getRotation());

		m_actor = m_physicsPtr->createRigidActor((entity->getTranslation() + meshTranslation + boundsCenter), Quaternion(XMQuaternionMultiply(meshComponent->getRotation(), entity->getRotation())), dynamic, this, sceneID, forceMakeKinematic);
		bool addGeom = true;
		m_centerOffset = boundsCenter;
		if (this->canAddGeometry())
		{
			if (!unique)
			{
				if (scale.x == 1 && scale.y == 1 && scale.z == 1)
				{
					addGeom = false;
					m_shape = m_physicsPtr->getSharedShape(name);
					if (m_shape)
					{
						m_physicsPtr->addShapeToActor(m_actor, m_shape);
					}
					else //Create shape and add shape for sharing
					{
						geometry = addGeometryByModelData(geometryType, meshComponent, physicsMaterialName, true, loaded);
						m_shape = m_physicsPtr->createAndSetShapeForActor(m_actor, geometry, physicsMaterialName, unique, scale);
						m_physicsPtr->addShapeForSharing(m_shape, name);
					}

				}

			}
			if (addGeom)
			{
				geometry = addGeometryByModelData(geometryType, meshComponent, physicsMaterialName, false, loaded);
				m_shape = m_physicsPtr->createAndSetShapeForActor(m_actor, geometry, physicsMaterialName, unique, scale);
				if(!loaded)
					delete geometry;
			}

		}
		

	}

	PxControllerBehaviorFlag::Enum getBehaviorFlag()
	{
		PxControllerBehaviorFlag::Enum flag;
		m_slide ? flag = PxControllerBehaviorFlag::eCCT_SLIDE : flag = PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;

		return flag;
	}

	void setSlide(bool shouldSlide)
	{
		m_slide = shouldSlide;
	}

	void makeKinematic()
	{
		if (!m_kinematic)
		{
			m_kinematic = true;
			if(m_dynamic)
				m_physicsPtr->makeActorKinematic(static_cast<PxRigidBody*>(this->m_actor));
		}

	}

	void makeKinematicDynamic(float newMass)
	{
		if (m_kinematic)
		{
			if (m_dynamic)
			{
				m_physicsPtr->makeKinematicActorDynamic(static_cast<PxRigidBody*>(m_actor), newMass);
				m_kinematic = false;
			}
		}
	}

	void kinematicMove(XMFLOAT3 destination, XMFLOAT4 quaternionRotation = {0.f, 0.f, 0.f, 1.f })
	{
		if (m_kinematic && m_dynamic)
			m_physicsPtr->kinematicMove(static_cast<PxRigidDynamic*>(m_actor), destination, quaternionRotation);
		else
			ErrorLogger::get().logError(L"Trying to kinematicMove actor that is not kinematic and/or dynamic");
	}

	void initActor(int sceneID, Entity* entity, bool dynamic)
	{
		m_dynamic = dynamic;
		m_transform = entity;
		if (!m_actor)
		{
			m_actor = m_physicsPtr->createRigidActor(entity->getTranslation(), entity->getRotation(), dynamic, this, sceneID);
		}
		else
		{
			ErrorLogger::get().logError("Trying to initialize an already initalized actor. Nothing happened.");
		}
	}

	void addBoxShape(XMFLOAT3 halfExtends, std::string materialName = "default", bool unique = true)
	{
		PxGeometryHolder geometry;
		if (canAddGeometry())
		{
			geometry = PxBoxGeometry(PxVec3(halfExtends.x, halfExtends.y, halfExtends.z));
			m_shape = m_physicsPtr->createAndSetShapeForActor(m_actor, geometry, materialName, unique);
		}
	}

	void addSphereShape(float radius, std::string materialName = "default", bool unique = true)
	{
		PxGeometryHolder geometry;
		if (canAddGeometry())
		{
			geometry = PxSphereGeometry(radius);
			m_shape = m_physicsPtr->createAndSetShapeForActor(m_actor, geometry, materialName, unique);
		}
	}

	void addPlaneShape(std::string materialName = "default", bool unique = true)
	{
		PxGeometryHolder geometry;
		if (canAddGeometry(true))
		{
			geometry = PxPlaneGeometry();
			m_shape = m_physicsPtr->createAndSetShapeForActor(m_actor, geometry, materialName, unique);
		}
	}

	void addCapsuleShape(float radius, float halfHeight, std::string materialName = "default", bool unique = true)
	{
		PxGeometryHolder geometry;
		if (canAddGeometry())
		{
			geometry = PxCapsuleGeometry(radius, halfHeight);
			m_shape = m_physicsPtr->createAndSetShapeForActor(m_actor, geometry, materialName, unique);
		}
	}


	PxGeometry* addGeometryByModelData(PxGeometryType::Enum geometry, const MeshComponent* meshComponent, std::string materialName, bool saveGeometry, bool &outLoaded)
	{
		XMFLOAT3 min, max;
		PxGeometry* bb = nullptr;
		meshComponent->getMeshResourcePtr()->getMinMax(min, max);
		
		std::string name = meshComponent->getFilePath() + std::to_string(geometry);
		bb = m_physicsPtr->getGeometry(name);

		if (!bb)
		{
			bb = createPrimitiveGeometry(geometry, min, max, meshComponent->getMeshResourcePtr());
			if (saveGeometry)
				m_physicsPtr->addGeometry(name, bb);
		}
		else
			outLoaded = true;
		
		return bb;
	}


	void setPhysicsMaterial(std::string phycisMaterialName)
	{
		m_physicsPtr->setShapeMaterial(m_shape, phycisMaterialName);

	}

	void setMass(float mass)
	{
		m_dynamic ? m_physicsPtr->setMassOfActor(m_actor, mass) : ErrorLogger::get().logError("Trying to change mass on a static actor.");
	}

	void clearForce()
	{
		if (m_dynamic)
		{
			m_physicsPtr->clearForce(static_cast<PxRigidDynamic*>(m_actor));
		}
	}

	void addForce(XMFLOAT3 forceAdd, bool massIndependant = true)
	{
		PxVec3 force(forceAdd.x, forceAdd.y, forceAdd.z);
		if (m_dynamic)
		{
			PxRigidDynamic* rigid = static_cast<PxRigidDynamic*>(m_actor);
			rigid->addForce(force, massIndependant ? PxForceMode::eVELOCITY_CHANGE : PxForceMode::eACCELERATION, true);
		}
		else
			ErrorLogger::get().logError("Tried to add force to static physicsComponent!");
	}

	void addForce(XMVECTOR forceAdd, bool massIndependant = true)
	{
		XMFLOAT3 forceFloat3;
		XMStoreFloat3(&forceFloat3, forceAdd);

		this->addForce(forceFloat3, massIndependant);
	}

	// Update
	void update(float dt) override 
	{
		//IF we check if it is static, We don't mirror the transform in the first place.
		if (m_dynamic || !m_hasMirrored)
		{
			Vector3 theMeshOffset;
			if (m_hasEntRot)
				theMeshOffset = XMVector3Rotate(m_meshOffset, m_transform->getRotation());
			else
				theMeshOffset = m_meshOffset;

			m_transform->setPosition(this->getActorPosition() - m_centerOffset - theMeshOffset);
			if (m_controllRotation)
				m_transform->setRotationQuat(this->getActorQuaternion());

			m_hasMirrored = true;
		}

	}

	XMFLOAT3 getActorPosition()
	{
		if (m_actor)
			return XMFLOAT3(m_actor->getGlobalPose().p.x, m_actor->getGlobalPose().p.y, m_actor->getGlobalPose().p.z);
		else
			return XMFLOAT3(0, 0, 0);
	}

	Vector3 getCenterOffset()
	{
		return m_centerOffset;
	}

	XMFLOAT4 getActorQuaternion()
	{
		return XMFLOAT4(m_actor->getGlobalPose().q.x, m_actor->getGlobalPose().q.y, m_actor->getGlobalPose().q.z, m_actor->getGlobalPose().q.w);
	}

	void controllRotation(bool shouldControllRotation)
	{
		m_controllRotation = shouldControllRotation;
	}

	void setPosition(XMFLOAT3 pos)
	{
		m_physicsPtr->setPosition(m_actor, pos);
	}

	void setRotation(XMFLOAT4 rotQ)
	{
		m_physicsPtr->setRotation(m_actor, rotQ);
	}

	void setPose(XMFLOAT3 pos, XMFLOAT4 rotQ)
	{
		m_physicsPtr->setGlobalTransform(m_actor, pos, rotQ);
	}

	void setVelocity(Vector3 velocity)
	{
		m_physicsPtr->setVelocity(static_cast<PxRigidBody*>(m_actor), velocity);
	}

	bool checkGround(Vector3 origin, Vector3 unitDirection, float distance)
	{
		return m_physicsPtr->castRay(origin, unitDirection, distance);
	}
};

