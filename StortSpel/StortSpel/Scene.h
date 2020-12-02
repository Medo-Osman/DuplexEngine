#pragma once
#include "Player.h"
#include "Engine.h"
#include "GUIHandler.h"
#include "Boss.h"
#include <map>
#include"ParticleComponent.h"

enum class ScenesEnum
{
	LOBBY,
	START,
	ARENA,
	MAINMENU,
	ENDSCENE,
};

enum PrefabType
{
	PARIS_WHEEL,
	FLIPPING_PLATFORM,
	SWEEPING_PLATFORM,
	PICKUP,
	SCORE,
	pfCHECKPOINT,
	SLOWTRAP,
	PUSHTRAP,
	BARRELDROP,
	GOAL_TRIGGER,
	SWINGING_HAMMER,
	pfSKYBOX
};

class Scene : public PhysicsObserver, public BossObserver
{
private:
	int m_sceneID = -1;
	int m_tempParticleID = 0;
	//std::unordered_map<std::string, Entity*> m_entities;
	Camera* m_camera;
	int nrOfParisWheels = 0;
	void createParisWheel(Vector3 position, float rotation, float rotationSpeed = 20, int nrOfPlatforms = 4);
	int nrOfFlippingPlatforms = 0;
	void createFlippingPlatform(Vector3 position, Vector3 rotation, float upTime = 3, float downTime = 3);
	int m_nrOfStaticPlatforms = 0;
	void createStaticPlatform(Vector3 position, Vector3 rotation, Vector3 scale, std::string meshPath, std::wstring texPath = L"GrayTexture.png");
	int m_nrOfSweepingPlatforms = 0;
	void createSweepingPlatform(Vector3 startPos, Vector3 endPos);
	void createTimedSweepPlatform(Vector3 startPos, Vector3 endPos, bool startEnd, float interval);
	int m_nrOfSpotLight = 0;
	void createSpotLight(Vector3 position, Vector3 rotation, Vector3 color, float intensity);
	int m_nrOfPointLight = 0;
	void createPointLight(Vector3 position, Vector3 color, float intensity);
	int m_nrSwingningHammers = 0;
	void createSwingingHammer(Vector3 position, Vector3 rotation, float swingSpeed);

	void createSkybox(std::wstring textureName = std::wstring(L"Skybox_Texture.dds"));

	//---------------------------------------------------------------Boss sstuff
	UINT m_nrOfProjectiles = 0;
	float m_projectileLifeTime = 10.f;
	void createProjectile(Vector3 origin, Vector3 dir, float speed);
	void checkProjectiles();

	UINT m_nrOfLasers = 0;
	void createLaser(BossStructures::BossActionData data);
	void checkLasers(float dt);

	UINT m_nrOfDisplacedPlatforms = 0;
	void checkPlatforms(float dt);
	void displacePlatform(Entity* entity);
	void physicallyMovePlatform(Entity* entity);
	bool findPlatformAlready(Entity* entity);

	GUIImageStyle imageStyle;
	int m_bossHP_barGuiIndex = 0;
	int m_bossHP_barBackgroundGuiIndex = 0;
	int m_endBossAtPecentNrOfStarts = 0;
	void removeBoss();
	void createPortal();
	void createEndScenePortal();

	//For projectiles
	std::unordered_map<UINT, Entity*> m_projectiles;

	//For lasers
	std::unordered_map<UINT, BossStructures::BossLaser*> m_lasers;

	//Displaced platforms from the grid
	std::unordered_map<UINT, BossStructures::PlatformDisplace*> m_displacedPlatforms;
	std::vector<Vector3> deferredPointInstantiationList;
	//---------------------------------------------------------------End of boss stuff
	

	Player* m_player;

	Material ObjectSpaceGrid;	// Temp global grid material

	Vector3 m_sceneEntryPosition;

	float m_nightVolume;
	float m_nightSlide;

	unsigned int long m_meshCount = 0;
	unsigned int long m_lightCount = 0;

	Timer m_despawnBarrelTimer;

	std::vector<std::pair<int, std::string>> m_scores;

	std::unordered_map<std::string, Entity*> m_entities;
	std::unordered_map<unsigned int long, MeshComponent*> m_meshComponentMap;
	std::unordered_map<std::string, LightComponent*> m_lightComponentMap;
	std::vector<ParticleComponent*> m_tempParticleComponent;

	void sendPhysicsMessage(PhysicsData& physicsData, bool& removed);
	int m_nrOfScore = 400;
	
	int m_nrOfPickups = 0;
	void addPickup(const Vector3& position, const int tier = 1, std::string name = "");
	void loadPickups();
	void loadScore();
	void addScore(const Vector3& position, const int tier = 1, std::string name = "");
	void addCheckpoint(const Vector3& position);
	void addSlowTrap(const Vector3& position, Vector3 scale, Vector3 hitBox);
	void addPushTrap(Vector3 wallPosition1, Vector3 wallPosition2, Vector3 triggerPosition);
	void createParticleEntity(void* particleComponent, Vector3 position);

	//void addSlowTrap(const Vector3& position, Vector3 scale);
	//void addPushTrap(Vector3 wallPosition1, Vector3 wallPosition2, Vector3 triggerPosition);

	void addComponentFromFile(Entity* entity, char* compData, int sizeOfData, bool& needsDynamicPhys, bool& needsKinematicPhys);
	void addPrefabFromFile(char* params);

	const std::string m_LEVELS_PATH = "../res/levels/";

	int m_nrOfCheckpoints = 0;
	int m_nrOfBarrelDrops = 0;
	int m_nrOftraps = 0;
	int m_nrOfPlayers = 4;


	std::vector<PhysicsComponent*> deferredPhysicsInitVec;


	int startGameIndex = 0;
public:
	Boss* m_boss = nullptr;
	Scene();
	~Scene();
	bool hidescore = false;
	static void loadMainMenu(Scene* sceneObject, bool* finished);
	static void loadScene(Scene* sceneObject, std::string path, bool* finished);
	static void loadTestLevel(Scene* sceneObject, bool* finished);
	static void loadEndScene(Scene* sceneObject, bool* finished);
	static void loadLobby(Scene* sceneObject, bool* finished);
	static void loadArena(Scene* sceneObject, bool* finished);
	static void loadMaterialTest(Scene* sceneObject, bool* finished);
	static void loadBossTest(Scene* sceneObject, bool* finished);
	static void loadEmpty(Scene* sceneObject, bool* finished);
	static void loadAlmostEmpty(Scene* sceneObject, bool* finished);

	void onSceneLoaded();

	void updateScene(const float &dt);
	Vector3 getEntryPosition();
	Entity* getEntity(std::string key);
	Entity* addEntity(std::string identifier);
	void removeEntity(std::string identifier);
	void setScoreVec();
	void sortScore();
	int m_nrOfScorePlayerOne = 54;
	int m_nrOfScorePlayerTwo = 12;
	int m_nrOfScorePlayerThree = 31;
	
	void setPlayersPosition(Entity* entity);

	bool endSceneCamera = false;
	bool addedBarrel = false;
	bool gameStarted = false;





	bool addComponent(Entity* entity, std::string componentIdentifier, Component* component);
	void addMeshComponent(MeshComponent* component);
	void addLightComponent(LightComponent* component);
	void addBarrelDrop(Vector3 Position);

	int getSceneID();

	void removeLightComponent(LightComponent* component);
	void removeLightComponentFromMap(LightComponent* component);
	void createNewPhysicsComponent(Entity* entity, bool dynamic = false, std::string meshName = "", PxGeometryType::Enum geometryType = PxGeometryType::eBOX, std::string materialName = "default", bool isUnique = false);


	std::vector<std::pair<int, std::string>>* getScores();
	std::unordered_map<std::string, Entity*>* getEntityMap();
	std::unordered_map<std::string, LightComponent*>* getLightMap();
	std::unordered_map<unsigned int long, MeshComponent*>* getMeshComponentMap();
	// Inherited via BossObserver
	virtual void bossEventUpdate(BossMovementType type, BossStructures::BossActionData data) override;

};
