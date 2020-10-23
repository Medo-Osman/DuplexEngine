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
	/*Entity* tent = addEntity("tent");
	if (tent)
	{
		addComponent(tent, "mesh", new MeshComponent("BigTopTent_Cylinder.lrm", Material({ L"T_CircusTent_D.png" })));
		tent->rotate({ XMConvertToRadians(-90.f), 0.f, 0.f });
		tent->move({ -10.f, 0.f, 0.f });

		this->createNewPhysicsComponent(tent, true, "");
	}*/

	// Floor
	Material gridTest = Material({ L"T_GridTestTex.bmp" });
	Entity* floor = engine->addEntity("floor");
	if (floor)
	{
		engine->addComponent(floor, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		floor->scale({ 300, 2,300 });
		floor->translate({ 0,-2,0 });
		engine->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
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

	// Flipping Cube 
	Entity* FlippingCube = engine->addEntity("FlippingCube");
	if (FlippingCube)
	{
		engine->addComponent(FlippingCube, "mesh",
			new MeshComponent("testCube_pCube1.lrm", Material({ L"DevTexture2m.png" })));
		FlippingCube->translate({ 5.f, 0.f, 15.f });
		FlippingCube->scale({ 4, 1, 4 });
		engine->addComponent(FlippingCube, "flipp",
			new FlippingComponent(dynamic_cast<Transform*>(FlippingCube), 3, 3));
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

	Entity* skelTest = engine->addEntity("skeleton-test");
	if (skelTest)
	{
		AnimatedMeshComponent* a1 = new AnimatedMeshComponent("skelTestStairs_stairs.lrsm", ShaderProgramsEnum::SKEL_ANIM);
		a1->translate({ 0.f, 0.f, 0.f });
		engine->addComponent(skelTest, "skeleton mesh", a1);
		
		AnimatedMeshComponent* a2 = new AnimatedMeshComponent("testTania_tania_geo.lrsm", ShaderProgramsEnum::SKEL_ANIM);
		a2->scaleUniform(0.02f);
		a2->translate({ 8.f, 0.f, 0.f });
		engine->addComponent(skelTest, "skeleton mesh", a2);
		
		AnimatedMeshComponent* a4 = new AnimatedMeshComponent("skelTestBaked_pCube1.lrsm", ShaderProgramsEnum::SKEL_ANIM);
		a4->translate({ 16.f, 0.f, 0.f });
		engine->addComponent(skelTest, "skeleton animation test", a4);
		
		a4->playAnimation("skelTestBaked",true);

		skelTest->translate({ 0.f, 1.5f, -20.f });
	}

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