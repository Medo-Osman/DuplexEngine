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

	//player
	/*m_player = engine->addEntity("m_player");
	if (m_player)
	{
		engine->addComponent(m_player, "mesh", new MeshComponent("testTania_tania_geo.lrm", ShaderProgramsEnum::TEMP_TEST));
		m_entities["m_player"]->translation({ 5, 10.f, 0 });
	}*/

	//tent
	entity = engine->addEntity("tent");
	if (entity)
	{
		m_entities["tent"] = entity;
		engine->addComponent(entity, "mesh", new MeshComponent("BigTopTent_Cylinder.lrm", Material({ L"T_CircusTent_D.png" })));
		entity->rotate({ XMConvertToRadians(-90.f), 0.f, 0.f });
		entity->move({ -10.f, 0.f, 0.f });

		engine->createNewPhysicsComponent(entity, true, "", PxGeometryType::eBOX, "default", false);
	}

	//floor
	entity = engine->addEntity("floor");
	if (entity)
	{
		m_entities["floor"] = entity;
		engine->addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", Material({ L"T_CircusTent_D.png" })));
		entity->scale({ 300,0.1,300 });
		entity->move({ 0,-0.6,0 });
		engine->createNewPhysicsComponent(entity, false, "", PxGeometryType::eBOX, "earth", false);
	}

	Material gridTest = Material({ L"T_GridTestTex.bmp" });

	//Cube 2
	entity = engine->addEntity("cube-test2");
	if (entity)
	{

		m_entities["cube-test2"] = entity;
		engine->addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", gridTest));
		entity->scaleUniform({ 3.f });
		entity->move({ 0.f, 5.f, 5.f });
		entity->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
		engine->createNewPhysicsComponent(entity, true, "", PxGeometryType::eSPHERE, "default", false);
	}

	//Cube with sphere shape
	entity = engine->addEntity("cube-test3");
	if (entity)
	{

		m_entities["cube-test3"] = entity;
		engine->addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm", ShaderProgramsEnum::TEMP_TEST));
		entity->scaleUniform({ 3.f });
		entity->move({ -10.f, 5.f, 5.f });
		entity->rotate({ 0.f, XMConvertToRadians(-45.f), XMConvertToRadians(-45.f) });
		engine->addComponent(entity, "physics", new PhysicsComponent(&Physics::get()));
		PhysicsComponent* physicsComp = static_cast<PhysicsComponent*>(entity->getComponent("physics"));
		physicsComp->initActor(entity, false);
		physicsComp->addSphereShape(2.f);
	}


	entity = engine->addEntity("testXwing");
	if (entity)
	{

		m_entities["testXwing"] = entity;
		engine->addComponent(entity, "xwingtestmesh", new MeshComponent("xWingFbx_xwing.lrm", Material({ L"T_tempTestXWing.png" })));
		entity->move({ 15.f, 1.5f, -3.f });
	}

	// Platforms
	for (int i = 0; i < 5; i++)
	{
		entity = engine->addEntity("cube-test" + std::to_string(i));

		m_entities["cube-test"] = entity;
		if (entity)
		{
			engine->addComponent(entity, "mesh", new MeshComponent("testCube_pCube1.lrm"));
			entity->scale({ 3,0.2,5 });
			entity->move({ 10.f + (float)i * 3.f, .2f + (float)i, 15.f });
			//defualt values for this function
			engine->createNewPhysicsComponent(entity,false, "", PxGeometryType::eBOX, "default", false);
		}
	}

	// Rotating Cube
	Entity* centerCube = engine->addEntity("centerCube");
	if (centerCube)
	{
		engine->addComponent(centerCube, "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		centerCube->move({ 0.f, 5.f, 0.f });
		centerCube->rotate({ 0.5f, 0, 0 });
	}

	// Rotating Cube
	m_entities["RotatingCube"] = engine->addEntity("RotatingCube");
	if (m_entities["RotatingCube"])
	{

		engine->addComponent(m_entities["RotatingCube"], "mesh",
			new MeshComponent("testCube_pCube1.lrm"));
		engine->addComponent(m_entities["RotatingCube"], "rotate",
			new RotateAroundComponent(centerCube->getTranslation(), centerCube->getRotationMatrix(), dynamic_cast<Transform*>(m_entities["RotatingCube"]), 5));

	}

	// Skybox
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