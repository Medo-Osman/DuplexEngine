#include "3DPCH.h"
#include "Player.h"


Player::Player()
{
	

	m_movementVector = XMVectorZero();
}

void Player::setStates(std::vector<State> states)
{
	 
	m_movementVector = XMVECTOR();
	for (std::vector<int>::size_type i = 0; i < states.size(); i++)
	{
		switch (states[i])
		{
		case WALK_LEFT:
			m_movementVector += m_cameraTransform->getRightVector() * -1;
			break;
		case WALK_RIGHT:
			m_movementVector += m_cameraTransform->getRightVector();
			break;
		case WALK_FORWARD:
			m_movementVector += m_cameraTransform->getForwardVector();
			break;
		case WALK_BACKWARD:
			m_movementVector += m_cameraTransform->getBackwardVector();
			break;
		default:
			break;
		}
	}
	
	
		
}

float lerp(float a, float b, float t)
{
	return a + (t * (b - a));
}

//To do: Implement with physics objects.
void Player::updatePlayer(const float& dt)
{
	m_playerEntity->move(m_movementVector * dt * m_playerSpeed);

	m_movementVector = XMVector3Normalize(m_movementVector);

	if(Vector3(m_movementVector).LengthSquared() > 0) //Only update when moving
		angleY = XMVectorGetY(XMVector3AngleBetweenNormals(XMVectorSet(0,0,1,0), m_movementVector));

	//if this vector has posisitv value the character is facing the positiv x axis, fixed to check against z axis and not camera forward
	if (XMVectorGetY(XMVector3Cross(m_movementVector, XMVectorSet(0,0,1,0))) > 0.0f)
	{
		angleY = -angleY;
	}

	//Quaternion currentRot(Vector3(0, m_playerEntity->getRotation().y, 0), 1);
	//currentRot.CreateFromAxisAngle(Vector3(0, 1, 0), m_playerEntity->getRotation().y);

	//Quaternion targetRot(Vector3(0, angleY, 0), 1);
	//targetRot.CreateFromAxisAngle(Vector3(0, 1, 0), angleY);

	
	//Quaternion slerpedRot = XMQuaternionLerp(currentRot, targetRot, 0.1f);

	//Quaternion slerpedRot = Quaternion::Slerp(currentRot, targetRot, 0.1f);
	//slerpedRot.Normalize();
	//float lerpedAngle = lerp(m_playerEntity->getRotation().y, angleY, dt / 0.1f);

	///*if(Vector3(m_movementVector).LengthSquared() > 0)
	//	m_playerEntity->rotate(Vector3(0, angleY, 0));*/

	//float rotY = std::asin(2 * (slerpedRot.x * slerpedRot.z + slerpedRot.w * slerpedRot.y));

	/*
	var yaw = atan2(2.0*(q.y*q.z + q.w*q.x), q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
	var pitch = asin(-2.0*(q.x*q.z - q.w*q.y));
	var roll = atan2(2.0*(q.x*q.y + q.w*q.z), q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);
	*/
	
	//float myAngle = std::asin(-2*(slerpedRot.x*slerpedRot.z - slerpedRot.w * slerpedRot.y));
	//XMQuaternionToAxisAngle(&XMVectorSet(1, 0, 0, 0), &myAngle, slerpedRot);
	
	//ErrorLogger::get().logError(std::string(std::to_string(XMConvertToDegrees(angleY))).c_str());


	//Quaternion currentRotation = m_playerEntity->getQuaternion();
	//Quaternion targetRot(Vector3(0, angleY, 0));
	//Quaternion slerped = Quaternion::Slerp(currentRotation, targetRot, dt/0.5);

	//m_playerEntity->setQuaternion(slerped, Vector3(0, angleY, 0));
	m_playerEntity->rotate(Vector3(0, angleY, 0));
	//m_playerEntity->setRotationMatrix(Matrix::CreateFromAxisAngle(Vector3(0, 1, 0), angleY), Vector3(0,angleY,0));

}

void Player::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
}

void Player::setCameraTranformPtr(Transform* transform)
{
	m_cameraTransform = transform;
}

Entity* Player::getPlayerEntity()
{
	return m_playerEntity;
}

void Player::inputUpdate(InputData& inputData)
{
	this->setStates(inputData.stateData);
}
