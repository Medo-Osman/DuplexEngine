#pragma once
#include "Input.h"

class Camera
{
private:
	XMVECTOR m_position;
	XMVECTOR m_rotation;
	const XMVECTOR upVector = { 0.0f, 1.0f, 0.0f, 0.0f };
	const XMVECTOR forwardVector = { 0.0f, 0.0f, 1.0f, 0.0f };
	const XMVECTOR rightVector = { 1.0f, 0.0f, 0.0f, 0.0f };

	XMVECTOR curUp;
	XMVECTOR curForward;
	XMVECTOR curRight;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

	void updateViewMatrix();
public:
	Camera();
	void setProjectionMatrix(const float &fov, const float &aspectRatio, const float &nearZ, const float &farZ);
	void setPosition(const XMVECTOR&pos);
	void setRotation(const XMVECTOR&rot);

	const XMVECTOR& getPosition() const;
	const XMFLOAT3 getFloat3Position() const;
	const XMVECTOR& getRotation() const;
	const XMFLOAT4 getFloat4Rotation() const;
	const XMMATRIX& getViewMatrix() const;
	const XMMATRIX& getProjectionMatrix() const;
	const XMFLOAT3 getForward() const;
	BoundingFrustum getFrustum();

	void controllCameraRotation(const MouseEvent& mEvent, const float& dt);
	void controllCameraPosition();




};