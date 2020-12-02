#pragma once
#include "BaseAction.h"
#include "Entity.h"

class ThunderAction : public BossStructures::BaseAction
{
private:
	int m_nrOfTargets;
	float m_warningTime;
	int m_chanseToSpwanStar;

	Entity* m_bossEntity;
	BossStructures::PlatformArray* m_platformArray = nullptr;
	BossStructures::BossActionData m_data;
	std::unordered_map<UINT, BossStructures::PlatformDisplace*>* m_removedPlatforms = nullptr;

	std::vector<Entity*> m_allPlatformsToMove;
	Timer m_timer;
	bool hasCallenThunder = false;
	bool hasFoundAllPlatforms = false;
	void warnPlatform(Entity* entity)
	{
		// Spawn particles on entity
		m_data.pointer0 = entity; //Get entity
		m_data.pointer1 = m_bossEntity;

		m_subjectPtr->Notify(BossMovementType::SpawnParticlesOnPlatform, m_data);
	}
	void movePlatform(Entity* entity, bool spawnStar)
	{
		m_data.pointer0 = entity; //Get entity
		m_data.origin = Vector3(entity->getTranslation().x, entity->getTranslation().y, entity->getTranslation().z); //Get Position of entity

		
		// Move entity
		m_subjectPtr->Notify(BossMovementType::MovePlatform, m_data);
		
		// Drop a star
		if (spawnStar == true)
		{
			m_data.origin = Vector3(entity->getTranslation().x, 4, entity->getTranslation().z); //Get Position of entity
			m_subjectPtr->Notify(BossMovementType::DropPoints, m_data);
		}
	}

public:
	ThunderAction(Entity* bossEntity, BossSubject* bossSubject, BossStructures::PlatformArray* platformArray, std::unordered_map<UINT, BossStructures::PlatformDisplace*>* removedPlatforms, int nrOfPlatformsToRemove = 5, float warningTime = 1.5, int chanseToSpwanStar = 20)
		:BaseAction(     bossEntity,              bossSubject)
	{
		m_movementActionType = BossMovementType::Thunder;
		m_platformArray = platformArray;
		m_removedPlatforms = removedPlatforms;
		m_nrOfTargets = nrOfPlatformsToRemove;
		m_warningTime = warningTime;
		m_chanseToSpwanStar = chanseToSpwanStar;
		m_bossEntity = bossEntity;
	};

	virtual void beginAction() override
	{		
		srand(time(0));
		m_timer.restart();
		//m_timer.start();
	};

	virtual void update(const float& dt) override
	{
		if (hasCallenThunder == false && hasFoundAllPlatforms == false)
		{
			for (int i = 0; i < m_nrOfTargets; i++)
			{
				int randX = rand() % m_platformArray->columns.size();
				int randY = rand() % m_platformArray->columns.size();

				Entity* platform = (*m_platformArray)[randX][randY];

				bool foundPlatform = true;
				for (auto displacedPlatformStruct : *m_removedPlatforms)
					if (displacedPlatformStruct.second->entity == platform)
					{
						foundPlatform = false;
						i--;
					}
				
				if (foundPlatform == true)
				{
					m_allPlatformsToMove.push_back(platform);
					warnPlatform(platform);
				}
			}
			hasFoundAllPlatforms = true;
		}

		if (m_timer.timeElapsed() >= m_warningTime)
		{
			for (int i = 0; i < m_nrOfTargets; i++)
			{
				bool spawnStar = false;
				int randomNumber = rand() % (100 / m_chanseToSpwanStar);

				if (randomNumber == 0)
					spawnStar = true;
				movePlatform(m_allPlatformsToMove.at(i), spawnStar);
			}

			hasCallenThunder = true;
		}
	};

	virtual bool isDone() override
	{
		return hasCallenThunder;
	};
};