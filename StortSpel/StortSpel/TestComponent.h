#include "3DPCH.h"
#ifndef TESTCOMPONENT_H
#define TESTCOMPONENT_H

#include "Component.h"

class TestComponents : public Component
{
private:
	std::string m_message;

public:
	TestComponents()
	{
		m_type = ComponentType::TEST;
		m_message = "Unvalid!";
	}
	~TestComponents() {}

	void initialize(std::string newMessage)
	{
		m_message = newMessage;
	}
	
	void outputMessage() const
	{
		OutputDebugStringA(m_message.c_str());
	}
};

#endif // !TESTCOMPONENT_H