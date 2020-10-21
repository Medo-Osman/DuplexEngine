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

	
	Entity* floor = engine->addEntity("floor"); // Floor:
	if (floor)
	{
		//m_entities["floor"] = floor;
		engine->addComponent(floor, "mesh", 
			new MeshComponent("testCube_pCube1.lrm", Material({ L"T_GridTestTex.bmp" })));

		floor->scale({ 20, 2, 20 });
		engine->createNewPhysicsComponent(floor, false, "", PxGeometryType::eBOX, "earth", false);
	}
// 1st:
	Vector3 position; Vector3 rotation; Vector3 scale; std::string meshPath;

//---------------------------------------------------------------------------------------------------------------------
	position = { 7.5, 0.5, 17.5 }; rotation = { 0, 0, 0 }; scale = { 5, 1, 15 }; meshPath = "testCube_pCube1.lrm";
	Entity* staticPlatform = engine->addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform)
	{ 
		engine->addComponent(staticPlatform, "mesh", new MeshComponent(meshPath.c_str(), Material({ L"GrayTexture.png" }))); staticPlatform->setPosition(position); staticPlatform->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)); staticPlatform->setScale(scale); engine->createNewPhysicsComponent(staticPlatform); 
	}
	m_nrOfStaticPlatforms++;
//---------------------------------------------------------------------------------------------------------------------
	/*position = { 7.5, 4, 24.5 }; rotation = { 0, 0, 0 }; scale = { 5, 6, 1 }; meshPath = "testCube_pCube1.lrm";
	Entity* staticPlatform1 = engine->addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform1)
	{
		engine->addComponent(staticPlatform1, "mesh", new MeshComponent(meshPath.c_str(), Material({ L"GrayTexture.png" }))); staticPlatform1->setPosition(position); staticPlatform1->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)); staticPlatform1->setScale(scale); engine->createNewPhysicsComponent(staticPlatform1);
	}
	m_nrOfStaticPlatforms++;*/
//---------------------------------------------------------------------------------------------------------------------
	position = { 7.5, 7.5, 31.5 }; rotation = { 0, 0, 0 }; scale = { 5, 1, 15 }; meshPath = "testCube_pCube1.lrm";
	Entity* staticPlatform2 = engine->addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform2)
	{
		engine->addComponent(staticPlatform2, "mesh", new MeshComponent(meshPath.c_str(), Material({ L"GrayTexture.png" }))); staticPlatform2->setPosition(position); staticPlatform2->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)); staticPlatform2->setScale(scale); engine->createNewPhysicsComponent(staticPlatform2);
	}
	m_nrOfStaticPlatforms++;
//---------------------------------------------------------------------------------------------------------------------
	position = { 2.5, 7.5, 36.5 }; rotation = { 0, 0, 0 }; scale = { 5, 1, 5 }; meshPath = "testCube_pCube1.lrm";
	Entity* staticPlatform3 = engine->addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform3)
	{
		engine->addComponent(staticPlatform3, "mesh", new MeshComponent(meshPath.c_str(), Material({ L"GrayTexture.png" }))); staticPlatform3->setPosition(position); staticPlatform3->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)); staticPlatform3->setScale(scale); engine->createNewPhysicsComponent(staticPlatform3);
	}
	m_nrOfStaticPlatforms++;
//--------------------------------------------------------------------------------------------------------------------- always cube
	position = { -7, 10, 36.5 }; rotation = { 0, 90, 0 }; scale = { 1, 1, 1 }; meshPath = "SquarePlatform.lrm";
	Entity* staticPlatform4 = engine->addEntity("banana");
	if (staticPlatform4)
	{
		std::string print;
		print = "\nAttempting to spawn: " + meshPath + "\n";
		OutputDebugStringA(print.c_str());

		Material mat = Material({ L"GrayTexture.png" });
		MeshComponent* newComp = new MeshComponent(meshPath.c_str(), mat);
		engine->addComponent(staticPlatform4, "mesh1", newComp); staticPlatform4->setPosition(position); staticPlatform4->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)); staticPlatform4->setScale(scale); engine->createNewPhysicsComponent(staticPlatform4);
		
		//MeshComponent* comp = dynamic_cast<MeshComponent*>(staticPlatform4->getComponent("mesh1"));
		//if (comp)
		//{
		//	print = meshPath + " : " + std::to_string(comp->getMeshResourcePtr()->vertCount) + "\n"; //should be around 300
		//	OutputDebugStringA(print.c_str());
		//}
	}
	m_nrOfStaticPlatforms++;
//---------------------------------------------------------------------------------------------------------------------
	position = { -19, 7.5, 36.5 }; rotation = { 0, 0, 0 }; scale = { 10, 1, 5 }; meshPath = "testCube_pCube1.lrm";
	Entity* staticPlatform5 = engine->addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform5)
	{
		engine->addComponent(staticPlatform5, "mesh", new MeshComponent(meshPath.c_str(), Material({ L"GrayTexture.png" }))); staticPlatform5->setPosition(position); staticPlatform5->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)); staticPlatform5->setScale(scale); engine->createNewPhysicsComponent(staticPlatform5);
	}
	m_nrOfStaticPlatforms++;








	//createStaticPlatform	(Vector3(7.5, 0.5, 17.5),	Vector3(0, 0, 0),		Vector3(5, 1, 15),		"testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(7.5, 4, 24.5),		Vector3(0, 0, 0),		Vector3(5, 6, 1),		"testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(7.5, 7.5, 31.5),	Vector3(0, 0, 0),		Vector3(5, 1, 15),		"testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(2.5, 7.5, 36.5),	Vector3(0, 0, 0),		Vector3(5, 1, 5),		"testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(-7, 10, 36.5),		Vector3(0, 90, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	//createStaticPlatform	(Vector3(-19, 7.5, 36.5),	Vector3(0, 0, 0),		Vector3(10, 1, 5),		"testCube_pCube1.lrm");

// 2nd:						Pos:						Rot:					Scale:					Other:
	//createStaticPlatform	(Vector3(-7.5, -4, 9.5),	Vector3(0, 0, 0),		Vector3(5, 6, 1),		"testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(-7.5, -6.5, 25),	Vector3(0, 0, 0),		Vector3(5, 1, 30),		"testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(-7.5, -5.5, 23),	Vector3(0, 0, 0),		Vector3(5, 1, 1),		"testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(-7.5, -5.5, 33),	Vector3(0, 0, 0),		Vector3(5, 1, 1),		"testCube_pCube1.lrm");
	//createStaticPlatform	(Vector3(-7.5, -2, 46),		Vector3(0, 0, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	//createStaticPlatform	(Vector3(-5, 2.5, 53.5),	Vector3(0, 45, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	//createStaticPlatform	(Vector3(2, 7.5, 56),		Vector3(0, 90, 0),		Vector3(1, 1, 1),		"SquarePlatform.lrm");
	//createStaticPlatform	(Vector3(12, 7.5, 56),		Vector3(0, 0, 0),		Vector3(5, 1, 5),		"testCube_pCube1.lrm");

	//createParisWheel		(engine, Vector3(-17.5, 7, 23), -30,	20);
	//createFlippingPlatform	(engine, Vector3(7, 3, 15));
	/////////////////////////////////////////////////////////////////////////////////////

	Entity* skybox = engine->addEntity("SkyBox");
	if (skybox)
	{
		Material skyboxMat;
		skyboxMat.addTexture(L"Skybox_Texture.dds", true);
		engine->addComponent(skybox, "cube", new MeshComponent("Skybox_Mesh_pCube1.lrm", ShaderProgramsEnum::SKYBOX, skyboxMat));

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

void Scene::createStaticPlatform(Vector3 position, Vector3 rotation, Vector3 scale, std::string meshPath)
{
	Engine* engine = &Engine::get();
	m_nrOfStaticPlatforms++;

	Entity* staticPlatform = engine->addEntity("StaticPlatform-" + std::to_string(m_nrOfStaticPlatforms));
	if (staticPlatform)
	{
		engine->addComponent(staticPlatform, "mesh",
			new MeshComponent(meshPath.c_str(), Material({ L"GrayTexture.png" })));
		
		staticPlatform->setPosition(position);
		staticPlatform->setRotation(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
		staticPlatform->setScale(scale);
		engine->createNewPhysicsComponent(staticPlatform);
	}
}
