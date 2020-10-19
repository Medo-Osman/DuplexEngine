#include "3DPCH.h"
#include "Scene.h"


Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::loadScene(std::string path)
{
	Engine* engine = &Engine::get();
	Entity* entity;
	/*
	Entity* cube1 = engine->addEntity("cube1");
	if (cube1)
	{
		engine->addComponent(cube1, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture1m.png" })));
		cube1->scaleUniform({ -1.f });
	}

	// Cube 2
	Entity* cube2 = engine->addEntity("cube2");
	if (cube2)
	{
		engine->addComponent(cube2, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture2m.png" })));
		cube2->translate({ 10.f, 0.5f, 0.f });
		cube2->scaleUniform({ -2.f });
	}

	// Cube 3
	Entity* cube3 = engine->addEntity("cube3");
	if (cube3)
	{
		engine->addComponent(cube3, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture3m.png" })));
		cube3->translate({ 20.f, 1.f, 0.f });
		cube3->scaleUniform({ -3.f });
	}

	// Cube 4
	Entity* cube4 = engine->addEntity("cube4");
	if (cube4)
	{
		engine->addComponent(cube4, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture4m.png" })));
		cube4->translate({ 30.f, 1.5f, 0.f });
		cube4->scaleUniform({ -4.f });
	}

	// Tent
	Entity* tent = addEntity("tent");
	if (tent)
	{
		addComponent(tent, "mesh", new MeshComponent("BigTopTent_Cylinder.lrm", Material({ L"T_CircusTent_D.png" })));
		tent->rotate({ XMConvertToRadians(-90.f), 0.f, 0.f });
		tent->move({ -10.f, 0.f, 0.f });

		this->createNewPhysicsComponent(tent, true, "");
	}

	// Flying Cube
	Entity* cube = engine->addEntity("cube-test2");
	if (cube)
	{
		engine->addComponent(cube, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		cube->scaleUniform({ 3.f });
		cube->translate({ 0.f, 5.f, 5.f });
		cube->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
		engine->createNewPhysicsComponent(cube, true, "", PxGeometryType::eSPHERE);
	}

	// Cube with sphere shape
	Entity* cubeSphereBB = engine->addEntity("cube-test3");
	if (cubeSphereBB)
	{
		engine->addComponent(cubeSphereBB, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
		cubeSphereBB->scaleUniform({ 3.f });
		cubeSphereBB->translate({ -10.f, 5.f, 5.f });
		cubeSphereBB->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
		engine->addComponent(cubeSphereBB, "physics", new PhysicsComponent());
		PhysicsComponent* physicsComp = static_cast<PhysicsComponent*>(cubeSphereBB->getComponent("physics"));
		physicsComp->initActor(cubeSphereBB, false);
		physicsComp->addSphereShape(2.f);
	}

	// XWing
	Entity* testXwing = engine->addEntity("testXwing");
	if (testXwing)
	{
		engine->addComponent(testXwing, "xwingtestmesh",
			new MeshComponent("xWingFbx_xwing.lrm", Material({ L"T_tempTestXWing.png" })));
		engine->addComponent(testXwing, "xwingtestmove",
			new SweepingComponent(dynamic_cast<Transform*>(testXwing), Vector3(0, 10, -5), Vector3(0, 10, 100), 20));
	}
	// Rotating Cube
	Entity* rotatingCube = engine->addEntity("RotatingCube");
	if (rotatingCube)
	{
		engine->addComponent(rotatingCube, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		engine->addComponent(rotatingCube, "rotate",
			new RotateAroundComponent(dynamic_cast<Transform*>(testXwing), testXwing->getRotationMatrix(), dynamic_cast<Transform*>(rotatingCube), 5));
	}
	// Rotating Cube 2
	Entity* rotatingCube2 = engine->addEntity("RotatingCube2");
	if (rotatingCube2)
	{
		engine->addComponent(rotatingCube2, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		engine->addComponent(rotatingCube2, "rotate",
			new RotateAroundComponent(dynamic_cast<Transform*>(rotatingCube), rotatingCube->getRotationMatrix(), dynamic_cast<Transform*>(rotatingCube2), 2, 40.f));
		rotatingCube2->scale({ 0.5f, 0.5f, 0.5f });
	}
	// Rotating Cube 3
	Entity* rotatingCube3 = engine->addEntity("RotatingCube3");
	if (rotatingCube3)
	{
		engine->addComponent(rotatingCube3, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		engine->addComponent(rotatingCube3, "rotate",
			new RotateAroundComponent(dynamic_cast<Transform*>(rotatingCube), rotatingCube->getRotationMatrix(), dynamic_cast<Transform*>(rotatingCube3), 2, 40.f, 180.f));
		rotatingCube3->scale({ 0.5f, 0.5f, 0.5f });
	}
	
	// Platforms
	for (int i = 0; i < 5; i++)
	{
		Entity* cube = engine->addEntity("cube-test" + std::to_string(i));
		if (cube)
		{
			engine->addComponent(cube, "mesh", new MeshComponent("testCube_pCube1.lrm"));
			cube->scale({ 3,0.2,5 });
			cube->translate({ 10.f + (float)i * 3.f, .2f + (float)i, 15.f });
			engine->createNewPhysicsComponent(cube);
		}
	}
	*/
	/////////////////////////////////////////////////////////////////////////////////////
	
	Entity* floor = engine->addEntity("floor"); // Floor:
	if (floor)
	{
		m_entities["floor"] = floor;
		engine->addComponent(floor, "mesh", 
			new MeshComponent("testCube_pCube1.lrm", Material({ L"T_GridTestTex.bmp" })));

		floor->scale({ 20, 2, 20 });
		engine->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}
// 1st:								 Pos:						Rot:					Scale:					Other:
	createStaticPlatform	(engine, Vector3(7.5, 0.5, 17.5),	Vector3(0, 0, 0),		Vector3(5, 1, 15),		"testCube_pCube1.lrm");
	createStaticPlatform	(engine, Vector3(7.5, 4, 24.5),		Vector3(0, 0, 0),		Vector3(5, 6, 1),		"testCube_pCube1.lrm");
	createStaticPlatform	(engine, Vector3(7.5, 7.5, 31.5),	Vector3(0, 0, 0),		Vector3(5, 1, 15),		"testCube_pCube1.lrm");
	createStaticPlatform	(engine, Vector3(2.5, 7.5, 36.5),	Vector3(0, 0, 0),		Vector3(5, 1, 5),		"testCube_pCube1.lrm");
	createStaticPlatform	(engine, Vector3(-7, 10, 36.5),		Vector3(0, 90, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	//createStaticPlatform	(engine, Vector3(-19, 7.5, 36.5),	Vector3(0, 0, 0),		Vector3(10, 1, 5),		"testCube_pCube1.lrm");

// 2nd:								 Pos:						Rot:					Scale:					Other:
	createStaticPlatform	(engine, Vector3(-7.5, -4, 9.5),	Vector3(0, 0, 0),		Vector3(5, 6, 1),		"testCube_pCube1.lrm");
	createStaticPlatform	(engine, Vector3(-7.5, -6.5, 25),	Vector3(0, 0, 0),		Vector3(5, 1, 30),		"testCube_pCube1.lrm");
	createStaticPlatform	(engine, Vector3(-7.5, -5.5, 23),	Vector3(0, 0, 0),		Vector3(5, 1, 1),		"testCube_pCube1.lrm");
	createStaticPlatform	(engine, Vector3(-7.5, -5.5, 33),	Vector3(0, 0, 0),		Vector3(5, 1, 1),		"testCube_pCube1.lrm");
	createStaticPlatform	(engine, Vector3(-7.5, -2, 46),		Vector3(0, 0, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	createStaticPlatform	(engine, Vector3(-5, 2.5, 53.5),	Vector3(0, 45, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	createStaticPlatform	(engine, Vector3(2, 7.5, 56),		Vector3(0, 90, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	createStaticPlatform	(engine, Vector3(12, 7.5, 56),		Vector3(0, 0, 0),		Vector3(5, 1, 5),		"testCube_pCube1.lrm");

	Entity* staticPlatform = engine->addEntity("abc");
	if (staticPlatform)
	{
		m_entities["abc"] = staticPlatform;
		engine->addComponent(staticPlatform, "mesh",
			new MeshComponent("SquarePlatform.lrm", Material({ L"GrayTexture.png" })));

		staticPlatform->setPosition(0, 10, 0);
		staticPlatform->setRotation(0, 0, 0);
		staticPlatform->setScale(1, 1, 1);
		engine->createNewPhysicsComponent(staticPlatform);
	}

	//createParisWheel		(engine, Vector3(-17.5, 7, 23), -30,	20);
	//createFlippingPlatform	(engine, Vector3(7, 3, 15));
	int stop = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	m_entities["SkyBox"] = engine->addEntity("SkyBox");
	if (m_entities["SkyBox"])
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		engine->addComponent(m_entities["SkyBox"], "cube", new MeshComponent("Skybox_Mesh_pCube1.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));

	}
}

void Scene::updateScene(const float& dt)
{
}


// Private functions:
void Scene::createParisWheel(Engine*& engine, Vector3 position, float rotation, float rotationSpeed, int nrOfPlatforms)
{
	nrOfParisWheels++;
	Entity* ParisWheel = engine->addEntity("ParisWheel-" + std::to_string(nrOfParisWheels));
	if (ParisWheel)
	{
		engine->addComponent(ParisWheel, "mesh",
			new MeshComponent("ParisWheel.lrm", Material({ L"GrayTexture.png" })));
		ParisWheel->setPosition(position);
		ParisWheel->setRotation(0, XMConvertToRadians(rotation), 0);
	}
	
	Entity* center = engine->addEntity("Empty-" + std::to_string(nrOfParisWheels));
	if (center)
	{
		center->setRotation(0, XMConvertToRadians(rotation), XMConvertToRadians(90));
		center->setPosition(position);
	}

	float test = 360 / nrOfPlatforms;
	for (int i = 0; i < 360; i+= test)
	{
		Entity* ParisWheelPlatform = engine->addEntity("ParisWheelPlatform-" + std::to_string(nrOfParisWheels) + "_" + std::to_string(i));
		if (ParisWheelPlatform)
		{
			engine->addComponent(ParisWheelPlatform, "mesh",
				new MeshComponent("ParisWheelPlatform.lrm", Material({ L"GrayTexture.png" })));
			engine->addComponent(ParisWheelPlatform, "rotate",
				new RotateAroundComponent(center, center->getRotationMatrix(), ParisWheelPlatform, 12, rotationSpeed, i));
			ParisWheelPlatform->setRotation(0, XMConvertToRadians(rotation), 0);
		}
	}
}

void Scene::createFlippingPlatform(Engine*& engine, Vector3 position, float upTime, float downTime)
{
	nrOfFlippingPlatforms++;
	Entity* flippingPlatform = engine->addEntity("FlippingCube-" + std::to_string(nrOfFlippingPlatforms));
	if (flippingPlatform)
	{
		engine->addComponent(flippingPlatform, "mesh",
			new MeshComponent("SquarePlatform.lrm", Material({ L"GrayTexture.png" })));
		engine->addComponent(flippingPlatform, "flipp",
			new FlippingComponent(flippingPlatform, upTime, downTime));

		//flippingPlatform->setRotation(0, XMConvertToRadians(180), 0);
		flippingPlatform->setPosition({ position });
	}
}

void Scene::createStaticPlatform(Engine* engine, Vector3 position, Vector3 rotation, Vector3 scale, std::string meshName)
{
	m_nrOfStaticPlatforms++;
	//std::string test = "StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms);
	Entity* staticPlatform = engine->addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform)
	{
		m_entities["StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms)] = staticPlatform;
		engine->addComponent(staticPlatform, "mesh",
			new MeshComponent(meshName.c_str(), Material({ L"GrayTexture.png" })));
		
		staticPlatform->setPosition(position);
		staticPlatform->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
		staticPlatform->setScale(scale);
		engine->createNewPhysicsComponent(staticPlatform);
	}
}
