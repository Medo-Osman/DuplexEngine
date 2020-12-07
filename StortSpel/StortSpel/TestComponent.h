#include "3DPCH.h"
#pragma once


#include "Component.h"

class TestComponent : public Component
{
private:
	std::string m_message;
	double arr[90000];
public:
	TestComponent()
	{
		m_type = ComponentType::TEST;
		m_message = "Not chosen!";
	}
	~TestComponent() 
	{
		std::cout << "\tDeleting testComp: " << m_identifier << std::endl;
	}

	// Initialization
	void init(std::string newMessage)
	{
		m_message = newMessage;
	}

	void outputMessage() const
	{
		OutputDebugStringA(m_message.c_str());
		OutputDebugStringA("\n");
	}

	// Update
	virtual void update(float dt) override
	{

	}
};
