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
		engine->addComponent(floor, "mesh", 
			new MeshComponent("testCube_pCube1.lrm", Material({ L"T_GridTestTex.bmp" })));

		floor->scale({ 20, 2, 20 });
		engine->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}
	//								 Pos:					Rot:	Other:
	createParisWheel		(engine, Vector3(-17.5, 7, 23), -30,	20);
	createStaticPlatform	(engine, Vector3(7, 4, 17),		0,		"SquarePlatform.lrm");
	createStaticPlatform	(engine, Vector3(10, 7, 27),	30,		"SquarePlatform.lrm");
	createStaticPlatform	(engine, Vector3(16.5, 11, 36),	14,		"SquarePlatform.lrm");
	//createFlippingPlatform	(engine, Vector3(7, 3, 15));

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
	nrOfParisWheels++;
}

void Scene::createFlippingPlatform(Engine*& engine, Vector3 position, float upTime, float downTime)
{
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
	nrOfFlippingPlatforms++;
}

void Scene::createStaticPlatform(Engine*& engine, Vector3 position, float rotation, std::string meshName)
{
	Entity* staticPlatform = engine->addEntity("StaticPlatform-" + std::to_string(nrOfStaticPlatforms));
	if (staticPlatform)
	{
		engine->addComponent(staticPlatform, "mesh",
			new MeshComponent(meshName.c_str(), Material({ L"GrayTexture.png" })));

		staticPlatform->setRotation(0, XMConvertToRadians(rotation), 0);
		staticPlatform->setPosition({ position });
		engine->createNewPhysicsComponent(staticPlatform);
	}
	nrOfStaticPlatforms++;
}
