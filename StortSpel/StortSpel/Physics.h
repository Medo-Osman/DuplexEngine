#pragma once
#include"3DPCH.h"
#include"PhysicsMaterial.h"
#include"cooking\PxCooking.h"
#include"VertexStructs.h"
static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;
using namespace physx;


struct PhysicsData
{
	TriggerType triggerType;
	int associatedTriggerEnum;
	std::string stringData;
	float floatData;
	int intData;
	void* pointer;
	std::string entityIdentifier;
	PhysicsData()
	{
		triggerType = TriggerType::UNDEFINED;
		associatedTriggerEnum = 0;
		stringData = "";
		floatData = 0;
		intData = 0;
		entityIdentifier = "";
	}
};
class PhysicsObserver {
public:
	PhysicsObserver() {};
	virtual ~PhysicsObserver() {};
	virtual void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity) = 0;
};

class PhysicsSubject {
public:
	virtual ~PhysicsSubject() {};
	virtual void Attach(PhysicsObserver* observer, bool reactOnTrigger, bool reactOnRemove) = 0;
	virtual void Detach(PhysicsObserver* observer, bool reactOnTrigger, bool reactOnRemove) = 0;
};

class Physics : public PxSimulationEventCallback, public PhysicsSubject
{
private:
	const char* m_host = "localhost";
	PxFoundation* m_foundationPtr;
	PxPhysics* m_physicsPtr;
	PxPvd* m_PvdPtr;
	PxPvdTransport* m_transport;
	PxCpuDispatcher* m_dispatcherPtr;
	PxScene* m_scenePtr;
	PxControllerManager* m_controllManager;
	PxCooking* m_cookingPtr;

	std::map<std::string, PxMaterial*> m_defaultMaterials;
	std::map<std::string, PxGeometry*> m_sharedGeometry;
	std::map<std::string, PxShape*> m_sharedShapes;
	std::map<std::string, PxTriangleMesh*> m_triangleMeshes;

	std::vector<PhysicsObserver*> m_reactOnTriggerObservers;
	std::vector<PhysicsObserver*> m_reactOnRemoveObservers;

	std::vector<PxActor*> m_actorsToRemoveAfterSimulation;

	bool m_recordMemoryAllocations = true;
	bool m_givenFirstScene = false;

	Physics()
	{
		m_foundationPtr = nullptr;
		m_physicsPtr = nullptr;
		m_PvdPtr = nullptr;
		m_dispatcherPtr = nullptr;
		m_scenePtr = nullptr;
		m_controllManager = nullptr;
		m_cookingPtr = nullptr;
	}

	void loadDefaultMaterials()
	{
		std::vector<PhysicsMaterial> physicMaterialVector;
		PhysicsMaterial::createDefaultMaterials(physicMaterialVector);
		for (int i = 0; i < physicMaterialVector.size(); i++)
		{
			this->addPhysicsMaterial(physicMaterialVector[i]);
		}
	}

	PxRigidActor* createDynamicActor(PxVec3 pos, PxQuat rot)
	{
		
		return m_physicsPtr->createRigidDynamic(PxTransform(pos, rot));
	}

	PxRigidActor* createStaticActor(PxVec3 pos, PxQuat rot)
	{
		return m_physicsPtr->createRigidStatic(PxTransform(pos, rot));
	}



	PxTriangleMesh* createTriangleMesh(int nrOfVerticies, PositionVertex vertexArray[], int nrOfIndicies, uint32_t indiciesArray[], Vector3 centerOffset = {0, 0, 0 })
	{
		PxVec3* vertArray = new PxVec3[nrOfVerticies];
		for (int i = 0; i < nrOfVerticies; i++)
		{
			PositionVertex v = vertexArray[i];
			vertArray[i] = PxVec3(v.position.x, v.position.y, v.position.z);
		}

		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = nrOfVerticies;
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = vertArray;

		meshDesc.triangles.count = nrOfIndicies / 3;
		meshDesc.triangles.stride = 3 * sizeof(uint32_t);
		meshDesc.triangles.data = indiciesArray;
		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		bool status = m_cookingPtr->cookTriangleMesh(meshDesc, writeBuffer, &result);
		if (!status)
			return NULL;

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		return m_physicsPtr->createTriangleMesh(readBuffer);
	}

	PxGeometryHolder scaleGeometry(PxGeometry* geometry, XMFLOAT3 scale)
	{
		PxGeometryHolder geometryHolder = *geometry;
		PxVec3 pxScale(scale.x, scale.y, scale.z);
		switch (geometryHolder.getType())
		{
		case physx::PxGeometryType::ePLANE:
			break;
		case physx::PxGeometryType::eCAPSULE:
			break;
		case physx::PxGeometryType::eSPHERE: //Jump into same since we can use box data
			geometryHolder.sphere().radius *= pxScale.maxElement();
			break;
		case physx::PxGeometryType::eBOX:
			 geometryHolder.box().halfExtents = geometryHolder.box().halfExtents.multiply(pxScale);
			break;
		}

		return geometryHolder;
	}

	std::vector<PxScene*> m_scenes;
	int m_nrOfThreads;
	Vector3 m_gravity;
	PxActor* m_latestTriggerInteractionActor;

	PxScene* createNewScene()
	{
		physx::PxSceneDesc sceneDesc(m_physicsPtr->getTolerancesScale());
		sceneDesc.gravity = PxVec3(m_gravity.x, m_gravity.y, m_gravity.z);
		m_dispatcherPtr = PxDefaultCpuDispatcherCreate(m_nrOfThreads);
		sceneDesc.cpuDispatcher = m_dispatcherPtr;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		sceneDesc.simulationEventCallback = this;
		return m_physicsPtr->createScene(sceneDesc);
	}
public:
	Physics(const Physics&) = delete;
	void operator=(Physics const&) = delete;
	static Physics& get()
	{
		static Physics instance;
		return instance;
	}

	~Physics()
	{
		for (auto& sharedGeometry : m_sharedGeometry)
			delete sharedGeometry.second;
	}

	void release()
	{
		m_controllManager->release();
		m_scenePtr->release();
		m_scenePtr = nullptr;
		m_cookingPtr->release();
		m_physicsPtr->release();
		m_PvdPtr->release();
		m_transport->release();
		delete m_dispatcherPtr;
		PxCloseExtensions();
		m_foundationPtr->release();
	}

	int getNewSceneID()
	{
		int id = (int)m_scenes.size();

		if (m_givenFirstScene)
		{
			m_scenes.emplace_back(createNewScene());

		}
		else
		{
			m_scenes.emplace_back(m_scenePtr);
			m_givenFirstScene = true;
		}

		return id;
	}

	void changeScene(int id)
	{
		if (id < m_scenes.size() && id > -1)
		{
			int pos = -1;
			
			for (int i = 0; i < m_scenes.size(); i++)
			{
				if (m_scenes[i] == m_scenePtr)
				{
					pos = i;
				}
			}

			for (int i = 0; i < m_actorsToRemoveAfterSimulation.size(); i++)
			{
				m_actorsToRemoveAfterSimulation.at(i)->release();
				m_actorsToRemoveAfterSimulation.at(i) = nullptr;
			}
			m_actorsToRemoveAfterSimulation.clear();

			m_controllManager->release();
			m_scenes[pos]->release();
			m_scenes[pos] = nullptr;

			m_scenePtr = m_scenes.at(id);
			m_controllManager = PxCreateControllerManager(*m_scenePtr);
		}
	}

	void Attach(PhysicsObserver* observer, bool reactOnTrigger, bool reactOnRemove)
	{
		if(reactOnTrigger)
			m_reactOnTriggerObservers.push_back(observer);
		if (reactOnRemove)
			m_reactOnRemoveObservers.push_back(observer);
	}

	void Detach(PhysicsObserver* observer, bool reactOnTrigger, bool reactOnRemove)
	{
		bool detatched = false;
		if (reactOnTrigger)
		{
			for (std::vector<int>::size_type i = 0; i < m_reactOnTriggerObservers.size() || !detatched; i++) {
				if (m_reactOnTriggerObservers[i] == observer)
				{
					detatched = true;
					m_reactOnTriggerObservers.erase(m_reactOnTriggerObservers.begin() + i);
				}
			}
		}

		if (reactOnRemove)
		{
			for (std::vector<int>::size_type i = 0; i < m_reactOnRemoveObservers.size() || !detatched; i++) {
				if (m_reactOnRemoveObservers[i] == observer)
				{
					detatched = true;
					m_reactOnRemoveObservers.erase(m_reactOnRemoveObservers.begin() + i);
				}
			}
		}

	}

	void init(const XMFLOAT3 &gravity = {0.0f, -9.81f, 0.0f}, const int &nrOfThreads = 1)
	{
		m_gravity = gravity;
		m_nrOfThreads = nrOfThreads;
		m_foundationPtr = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

		m_PvdPtr = PxCreatePvd(*m_foundationPtr);
		m_transport = physx::PxDefaultPvdSocketTransportCreate(m_host, 5425, 10);
		m_PvdPtr->connect(*m_transport, physx::PxPvdInstrumentationFlag::eALL);


		m_physicsPtr = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundationPtr,
			physx::PxTolerancesScale(), m_recordMemoryAllocations, m_PvdPtr);
		if (!m_physicsPtr)
			assert("PxCreatePhysics failed!");

		if (!PxInitExtensions(*m_physicsPtr, m_PvdPtr))
			assert("PxInitExtensions failed!");

		physx::PxSceneDesc sceneDesc(m_physicsPtr->getTolerancesScale());
		sceneDesc.gravity = PxVec3(gravity.x, gravity.y, gravity.z);
		m_dispatcherPtr = PxDefaultCpuDispatcherCreate(nrOfThreads);
		sceneDesc.cpuDispatcher = m_dispatcherPtr;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		sceneDesc.simulationEventCallback = this;
		m_scenePtr = m_physicsPtr->createScene(sceneDesc);
		m_controllManager = PxCreateControllerManager(*m_scenePtr);

		PxPvdSceneClient* pvdClient = m_scenePtr->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		PxCookingParams params = PxCookingParams(PxTolerancesScale());
		m_cookingPtr = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundationPtr, params);

		assert(m_cookingPtr);



		this->loadDefaultMaterials();
	}


	PxTriangleMesh* getTriangleMeshe(std::string nameOfMesh, int nrOfVerticies, PositionVertex vertexArray[], int nrOfIndicies, uint32_t indiciesArray[], Vector3 centerOffset)
	{
		if (m_triangleMeshes.find(nameOfMesh) != m_triangleMeshes.end())
		{
			return m_triangleMeshes[nameOfMesh];
		}
		else
		{
			return m_triangleMeshes[nameOfMesh] = (createTriangleMesh(nrOfVerticies, vertexArray, nrOfIndicies, indiciesArray, centerOffset));
		}
	}

	void clearForce(PxRigidDynamic* actor)
	{
		actor->clearForce();
		actor->clearTorque();
		actor->setLinearVelocity({ 0,0,0 });
		actor->setAngularVelocity({ 0,0,0 });

	}

	void addShapeForSharing(PxShape* shape, const std::string &name)
	{
		if (shape->isExclusive())
			ErrorLogger::get().logError("A shape being added for sharing is exclusive.");
		else
		{
			if (m_sharedShapes.find(name) == m_sharedShapes.end())
				m_sharedShapes[name] = shape;
		}
	}

	void onTrigger(PxTriggerPair* pairs, PxU32 count) //Will currently check for collisions with player and said trigger shape.
	{
		for (PxU32 i = 0; i < count; i++)
		{
			bool shouldBeRemoved = false;
			// ignore pairs when shapes have been deleted


			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

			//Checks
			PhysicsData* data = static_cast<PhysicsData*>(pairs[i].triggerActor->userData);
			PhysicsData* otherData = static_cast<PhysicsData*>(pairs[i].otherActor->userData);
			if (pairs[i].otherActor == m_controllManager->getController(0)->getActor() || (data->triggerType == TriggerType::PROJECTILE || otherData->triggerType == TriggerType::PROJECTILE))
			{
				for (size_t j = 0; j < m_reactOnTriggerObservers.size() && !shouldBeRemoved; j++)
				{
					m_reactOnTriggerObservers[j]->sendPhysicsMessage(*data, shouldBeRemoved);
					if (shouldBeRemoved)
					{
						bool stopLoop = false;
						for (size_t k = 0; k < m_reactOnRemoveObservers.size() && !stopLoop; k++)
						{
							m_reactOnRemoveObservers[k]->sendPhysicsMessage(*data, stopLoop);
						}
					}
					else
					{
						m_latestTriggerInteractionActor = pairs[i].triggerActor;
					}
				}
			}
		}
	}

	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
	void onWake(PxActor** actors, PxU32 count) {}
	void onSleep(PxActor** actors, PxU32 count) {}
	void onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs) 
	{
	}
	void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform * poseBuffer, const PxU32 count) {}


	PxShape* getSharedShape(const std::string &name)
	{
		if (m_sharedShapes.find(name) != m_sharedShapes.end())
			return m_sharedShapes[name];

		return nullptr;
	}

	PxShape* createAndSetShapeForActor(PxRigidActor* actor, PxGeometry* geometry, const std::string &materialName, const bool &unique, const XMFLOAT3 &scale = { 1, 1, 1 }, const bool trigger = false)
	{
		physx::PxMaterial* physicsMaterial = getMaterialByName(materialName);
		PxGeometryHolder scaledGeometry = *geometry;
		if (physicsMaterial == nullptr)
		{
			physicsMaterial = m_defaultMaterials["default"];
			physicsMaterial ? ErrorLogger::get().logError(std::string("Physical material:" + materialName + " does not exist, default is used.").c_str()) : assert("Default material has been changed or cannot be accessed, a recent change in code did this.");
		}
		if (scale.x != 1 || scale.y != 1 || scale.z != 1)
			scaledGeometry = scaleGeometry(geometry, scale);


		PxShape* shape = m_physicsPtr->createShape(scaledGeometry.any(), *physicsMaterial, unique);
		if (trigger)
		{
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		}
		actor->attachShape(*shape);
		return shape;
	}

	PxShape* createAndSetShapeForActor(PxRigidActor* actor, PxGeometryHolder geometry, const std::string &materialName, const bool &unique, const XMFLOAT3 &scale = { 1, 1, 1 }, const bool trigger = false)
	{
		physx::PxMaterial* physicsMaterial = getMaterialByName(materialName);
		if (physicsMaterial == nullptr)
		{
			physicsMaterial = m_defaultMaterials["default"];
			physicsMaterial ? ErrorLogger::get().logError("Physical material entered does not exist, default is used.") : assert("Default material has been changed or cannot be accessed, a recent change in code did this.");
		}

		PxShape* shape = m_physicsPtr->createShape(geometry.any(), *physicsMaterial, unique);
		if (trigger)
		{
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		}

		actor->attachShape(*shape);
		return shape;
	}

	PxMaterial* getMaterialByName(const std::string &name)
	{
		PxMaterial* materialPtr = nullptr;
		if (m_defaultMaterials.find(name) != m_defaultMaterials.end())
		{
			materialPtr = m_defaultMaterials[name];
		}
		return materialPtr;
	}

	void addShapeToActor(PxRigidActor* actor, PxShape* shape)
	{
		actor->attachShape(*shape);
	}

	void removeActor(PxActor* actor)
	{
		m_actorsToRemoveAfterSimulation.emplace_back(actor);
	}

	PxRigidActor* createRigidActor(const XMFLOAT3& position, const XMFLOAT4& quaternion, const bool& dynamic, void* physicsComponentPtr, int sceneID, bool needsToBeKinematic = false)
	{
		PxVec3 pos(position.x, position.y, position.z);
		PxQuat quat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

		PxRigidActor* actor = dynamic ? createDynamicActor(pos, quat) : createStaticActor(pos, quat);
		actor->userData = physicsComponentPtr;
		if(needsToBeKinematic) //If dynamic and trianglemesh is set this will be true.
			static_cast<PxRigidDynamic*>(actor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		m_scenes[sceneID]->addActor(*actor);
		return actor;
	}

	void setPosition(PxRigidActor* actor, const XMFLOAT3 &pos)
	{
		actor->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y, pos.z)));
	}

	void setRotation(PxRigidActor* actor, const XMFLOAT4 &rotationQuat)
	{
		actor->setGlobalPose(PxTransform(PxQuat(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w)));
	}

	void setGlobalTransform(PxRigidActor* actor, const XMFLOAT3 &pos,  const XMFLOAT4 &rotQ)
	{
		actor->setGlobalPose(PxTransform(pos.x, pos.y, pos.z, PxQuat(rotQ.x, rotQ.y, rotQ.z, rotQ.w)));
	}


	void setMassOfActor(PxRigidActor* actor, const float &weight)
	{
		static_cast<PxRigidDynamic*>(actor)->setMass(weight);
		//static_cast<PxRigidDynamic*>(actor)->;


	}

	void addPhysicsMaterial(const PhysicsMaterial &physicsMaterial)
	{
		if (m_defaultMaterials.find(physicsMaterial.name) == m_defaultMaterials.end())
		{
			PxMaterial* currentPhysXMaterial = m_defaultMaterials[physicsMaterial.name] = m_physicsPtr->createMaterial(physicsMaterial.staticFriction, physicsMaterial.dynamicFriction, physicsMaterial.restitution);
			currentPhysXMaterial->setFrictionCombineMode(physicsMaterial.frictionCombineMode);
			currentPhysXMaterial->setRestitutionCombineMode(physicsMaterial.restitutionCombineMode);
		}
		else
			ErrorLogger::get().logError(std::string("Trying to add a duplicate of material" + physicsMaterial.name).c_str());
	}

	void addPhysicsMaterial(const std::string &name, const float &staticFriction, const float &dynamicFriction, const float &restitution, const physx::PxCombineMode::Enum &frictionCombineMode, const physx::PxCombineMode::Enum &restitutionCombineMode)
	{
		this->addPhysicsMaterial(PhysicsMaterial(name, staticFriction, dynamicFriction, restitution, frictionCombineMode, restitutionCombineMode));
	}

	void setShapeMaterial(PxShape* shape, const std::string &materialName)
	{
		PxMaterial* materials[1];
		materials[0] = m_defaultMaterials[materialName];
		shape->isExclusive() ? shape->setMaterials(materials, 1) : ErrorLogger::get().logError("Trying to change material on shape that is not exclusive, this is not possible.");
	}

	void update(const float &dt)
	{
		
		m_scenePtr->simulate(dt);
		m_scenePtr->fetchResults(true);
		if (m_latestTriggerInteractionActor)
		{
			m_scenePtr->resetFiltering(*m_latestTriggerInteractionActor);
			m_latestTriggerInteractionActor = nullptr;
		}

		for (size_t i = 0; i < m_actorsToRemoveAfterSimulation.size(); i++)
		{
			PxActor* actor = m_actorsToRemoveAfterSimulation[i];
			actor->getScene()->removeActor(*actor, false);
		}
		m_actorsToRemoveAfterSimulation.clear();
	}

	PxGeometry* getGeometry(const std::string &geometryName)
	{
		if (m_sharedGeometry.find(geometryName) != m_sharedGeometry.end())
			return m_sharedGeometry[geometryName];
		else
			return nullptr;
	}

	void addGeometry(const std::string &geometryName, PxGeometry* geometry)
	{
		if (m_sharedGeometry.find(geometryName) == m_sharedGeometry.end())
			m_sharedGeometry[geometryName] = geometry;
		else
			ErrorLogger::get().logError("Trying to add already existing geometry to sharedGeometry map");
	}

	bool castRay(const SimpleMath::Vector3 &origin, const SimpleMath::Vector3 &unitDirection, const float &distance)
	{
		PxVec3 pOrigin(origin.x, origin.y, origin.z);
		PxVec3 pUnitDir(unitDirection.x, unitDirection.y, unitDirection.z);
		PxRaycastBuffer hit;                 // [out] Raycast results

		// Raycast against all static & dynamic objects (no filtering)
		// The main result from this call is the closest hit, stored in the 'hit.block' structure
		return m_scenePtr->raycast(pOrigin, pUnitDir, distance, hit);
	}

	bool castRay(const SimpleMath::Vector3& origin, const SimpleMath::Vector3& unitDirection, const float& distance, Vector3& OUT_position)
	{
		bool didHit;
		OUT_position = { -666, -1337, -420 };
		PxVec3 pOrigin(origin.x, origin.y, origin.z);
		PxVec3 pUnitDir(unitDirection.x, unitDirection.y, unitDirection.z);
		PxRaycastBuffer hit;                 // [out] Raycast results

		// Raycast against all static & dynamic objects (no filtering)
		// The main result from this call is the closest hit, stored in the 'hit.block' structure
		didHit = m_scenePtr->raycast(pOrigin, pUnitDir, distance, hit);
		if (hit.hasBlock)
		{
			PxVec3 hitPos = hit.block.position;
			OUT_position = Vector3(hitPos.x, hitPos.y, hitPos.z);
		}
		return didHit;
	}

	bool sphereIntersectionTest(const PxExtendedVec3& origin, const float& radius)
	{
		PxQueryFilterData fd;
		fd.flags |= PxQueryFlag::eANY_HIT;
		PxOverlapBuffer hit;            // [out] Overlap results
		PxSphereGeometry overlapShape(radius);  // [in] shape to test for overlaps
		PxTransform shapePose(origin.x, origin.y, origin.z);    // [in] initial shape pose (at distance=0)

		bool status = m_scenePtr->overlap(overlapShape, shapePose, hit, fd);
		return status;
	}

	bool hitSomething(Vector3 position, float radius, float halfHeight)
	{
		PxQueryFilterData fd;
		fd.flags |= PxQueryFlag::eANY_HIT;
		PxOverlapBuffer hit;            // [out] Overlap results
		PxCapsuleGeometry overlapShape(radius, halfHeight) ;  // [in] shape to test for overlaps
		PxTransform shapePose = PxTransform(PxVec3(position.x, position.y, position.z));    // [in] initial shape pose (at distance=0)


		bool status = m_scenePtr->overlap(overlapShape, shapePose, hit, fd);
		return status;
	}

	//Manager
	PxController* addCapsuleController(const XMFLOAT3 &position, const float &height, const float &radius, const std::string &materialName, PxControllerBehaviorCallback* controlBehavior)
	{
		PxController* capsuleController = nullptr;
		PxCapsuleControllerDesc ccd;
		ccd.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
		ccd.contactOffset = 0.001f;
		ccd.height = height;
		ccd.radius = radius;
		ccd.invisibleWallHeight = 0.f;
		ccd.maxJumpHeight = 0.f; //If invisibleWalLHeight is used, this parameter is used.
		ccd.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
		ccd.position = PxExtendedVec3(position.x, position.y, position.z);
		ccd.registerDeletionListener = true;
		ccd.slopeLimit = 0.707f;
		ccd.stepOffset = 0.5f;
		ccd.upDirection = PxVec3(0.f, 1.f, 0.f);
		ccd.userData = NULL;
		ccd.volumeGrowth = 1.5f;

		//Callbacks
		ccd.reportCallback = NULL;
		ccd.behaviorCallback = controlBehavior;

		//Actor
		ccd.density = 10.0;
		ccd.material = getMaterialByName(materialName);
		ccd.scaleCoeff = 0.8f;

		capsuleController = m_controllManager->createController(ccd);
		dynamic_cast<PxRigidBody*>(capsuleController->getActor())->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
		PxShape* shapes[1];
		PxRigidDynamic* actor = capsuleController->getActor();
		int nrOfShapes = actor->getNbShapes();
		actor->getShapes(shapes, 1 * sizeof(PxShape), 0);
		shapes[0]->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);

		return capsuleController;
	}

	void removeCharacterController(PxController* controller)
	{
		controller->release();
	}

	void purgeCharacterController()
	{
		m_controllManager->purgeControllers();
	}

	void setCapsuleSize(PxController* controller, const float &height)
	{
		controller->resize(height);
	}

	void setCapsuleRadius(PxController* controller, const float &radius)
	{
		PxCapsuleController* capsule = static_cast<PxCapsuleController*>(controller);
		capsule->resize(radius);

	}


	//Kinematic Actor
	void makeActorKinematic(PxRigidBody* actor)
	{
		actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
	}

	void makeKinematicActorDynamic(PxRigidBody* actor, float newMass)
	{
		actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, false);
		actor->setMass(newMass);
	}

	void setVelocity(PxRigidBody* actor, Vector3 velocity)
	{
		actor->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
	}

	void kinematicMove(PxRigidDynamic* actor, XMFLOAT3 destination, XMFLOAT4 quatRot)
	{
		actor->setKinematicTarget(PxTransform(destination.x, destination.y, destination.z, PxQuat(quatRot.x, quatRot.y, quatRot.z, quatRot.w)));
	}
};
