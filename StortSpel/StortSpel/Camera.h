#pragma once
#include "Input.h"
#include "Transform.h"

class Camera : public InputObserver
{
private:
	XMVECTOR m_position;
	XMVECTOR m_rotation;
	const XMVECTOR upVector = { 0.0f, 1.0f, 0.0f, 0.0f };
	const XMVECTOR forwardVector = { 0.0f, 0.0f, 1.0f, 0.0f };
	const XMVECTOR rightVector = { 1.0f, 0.0f, 0.0f, 0.0f };

	Transform m_transform;

	XMMATRIX m_viewMatrix;
	//FrustumCulling

	XMMATRIX m_projectionMatrix;

	bool m_newIncrements;
	XMVECTOR m_incrementRotation;
	XMVECTOR m_incrementPosition;

	float m_sensitivity = 0.1f;

	void updateViewMatrix();
	void updateViewMatrixEndScene();
public:
	Camera();
	~Camera();
	void initialize(const float& fov, const float& aspectRatio, const float& nearZ, const float& farZ);
	void setProjectionMatrix(const float &fov, const float &aspectRatio, const float &nearZ, const float &farZ);
	void setPosition(const XMVECTOR&pos);
	void setRotation(const XMVECTOR&rot);
	bool endSceneCamera = false;
	bool frustumCullingOn = true;

	Transform* getTransform();

	void increaseSensitivity();
	void DecreaseSensitivity();
	float getSensitivity();

	const XMVECTOR& getPosition() const;
	const XMFLOAT3 getFloat3Position() const;
	const XMVECTOR& getRotation() const;
	const XMFLOAT4 getFloat4Rotation() const;
	const XMMATRIX& getViewMatrix() const;
	const XMMATRIX& getProjectionMatrix() const;

	BoundingFrustum getFrustum();
	


	virtual void update(const float& dt);
	void inputUpdate(InputData& inputData);




};