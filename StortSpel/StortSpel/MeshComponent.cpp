#include "3DPCH.h"
#include "MeshComponent.h"


MeshComponent::MeshComponent(const char* filepath)
{
	m_type = ComponentType::MESH;
	m_resourcePointer = ResourceHandler::get().loadLRMMesh(filepath);
}
