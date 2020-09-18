#pragma once
#include"3DPCH.h"

static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;
using namespace physx;

class Physics
{

private:
	const char* m_host = "localhost";
	physx::PxFoundation* m_foundationPtr;
	physx::PxPhysics* m_physicsPtr;
	physx::PxPvd* m_PvdPtr;
	physx::PxCpuDispatcher* m_dispatcherPtr;
	physx::PxScene* m_scenePtr;

	PxRigidDynamic* m_boxBodyPtr; //For test, remove in other sprint where functionality is added.

	bool m_recordMemoryAllocations = true;

	void loadScene()
	{
		PxTransform pos(PxVec3(0.f, 0.f, -2.f));
		physx::PxMaterial* mat = m_physicsPtr->createMaterial(0.5f, 0.5f, 0.6f);
		PxShape* box = m_physicsPtr->createShape(PxBoxGeometry(PxVec3(2.f, 2.f, 2.f)), *mat);

		m_boxBodyPtr = m_physicsPtr->createRigidDynamic(pos);
		m_boxBodyPtr->attachShape(*box);
		PxRigidBodyExt::updateMassAndInertia(*m_boxBodyPtr, 10.0f);
		m_scenePtr->addActor(*m_boxBodyPtr);
	}


public:
	Physics()
	{
		m_foundationPtr = nullptr;
		m_physicsPtr = nullptr;
		m_PvdPtr = nullptr;
		m_scenePtr = nullptr;
	}
	~Physics()
	{
		m_scenePtr->release();
		m_physicsPtr->release();
		m_foundationPtr->release();
		m_PvdPtr->release();
	}
	void init(XMFLOAT3 gravity = {0.0f, -9.81f, 0.0f}, int nrOfThreads = 1)
	{
		m_foundationPtr = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

		m_PvdPtr = PxCreatePvd(*m_foundationPtr);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(m_host, 5425, 10);
		m_PvdPtr->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);


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
		m_scenePtr = m_physicsPtr->createScene(sceneDesc);

		PxPvdSceneClient* pvdClient = m_scenePtr->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		this->loadScene();
	}

	void update(float dt)
	{
		m_scenePtr->simulate(dt);
		m_scenePtr->fetchResults(true);

		//Task test of completion, gravity takes the box down in y axis.
		//PxVec3 pos = m_boxBodyPtr->getGlobalPose().p;
		//OutputDebugString(std::to_wstring(pos.y).c_str());
		//OutputDebugString(L"\n");
	}

};
