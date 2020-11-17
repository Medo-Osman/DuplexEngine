#pragma once
#include "3DPCH.h"
#include <functional>

enum class BossMovementType {NONE, ShootProjectile, DropPoints, MoveTo };

class BossSubject;

namespace BossStructures
{
	struct BossActionData
	{
		Vector3 direction;
		Vector3 origin;
		float speed = 0;
		void* pointer0 = nullptr;
	};

	struct ActionTimeData
	{
		Timer timer;
		float maxDuration = 5; //For how many seconds the action should be performed for
		float cooldownAfterAction = 0.2; //For many seconds long it should wait before
	};


	class BaseAction
	{
	protected:
		BossMovementType m_movementActionType = BossMovementType::NONE;
		Entity* m_bossEntity = nullptr;
		BossSubject* m_subjectPtr;

		ActionTimeData m_timeData;

	public:
		UINT m_actionID = -1; //Will look like garbage value, should raise eyebrows if not set.
		BaseAction(Entity* bossEntity, BossSubject* bossSubject)
		{
			m_bossEntity = bossEntity;
			m_subjectPtr = bossSubject;

		}

		ActionTimeData* getTimeData()
		{
			return &m_timeData;
		}

		BossMovementType getType()
		{
			return m_movementActionType;
		}

		void setType(BossMovementType type)
		{
			m_movementActionType = type;
		}

		virtual bool isDone() = 0;
		virtual void beginAction() = 0;
		virtual void update(const float& dt) = 0;
	};

	//Should allow you to do PlatformArray[index][otherIndex], cannot do a dynamic 2D-array natively.
	struct EntityArray //Holds entities
	{
		std::vector<Entity*> items;
		EntityArray(int count)
		{
			for (int i = 0; i < count; i++)
			{
				//Init to null
				items.push_back(nullptr);
			}
		}

		~EntityArray()
		{
			for (int i = 0; i < items.size(); i++)
			{
				if (items[i] != nullptr) //Might not exist if something went wrong.
				{
					delete items[i];
				}
			}
		}

		Entity*& operator[](int index) 
		{
			return items[index];
		}
	};
	struct PlatformArray //Holds entity arrays
	{
		std::vector<EntityArray*> columns;
		PlatformArray(int count)
		{
			for (int i = 0; i < count; i++)
			{
				columns.push_back(new EntityArray(count));
			}
		}

		~PlatformArray()
		{
			for (int i = 0; i < columns.size(); i++)
			{
				delete columns.at(i); //We know they exist already.
			}
		}


		EntityArray& operator[](int index)
		{
			return *columns.at(index);
		}
	};


}

class BossObserver {
public:
	BossObserver() {};
	virtual void bossEventUpdate(BossMovementType type, BossStructures::BossActionData) = 0;

	static int nr;
	int index = 0;
};

class BossSubject {
public:
	virtual ~BossSubject() {};
	virtual void Attach(BossObserver* observer) = 0;
	virtual void Detach(BossObserver* observer) = 0;
	virtual void Notify(BossMovementType type, BossStructures::BossActionData data) = 0;
};