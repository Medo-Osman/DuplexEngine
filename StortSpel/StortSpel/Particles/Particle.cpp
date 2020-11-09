#include"Particle.h"


Microsoft::WRL::ComPtr<ID3DBlob> Particle::m_vertexShaderBlob = nullptr;
Microsoft::WRL::ComPtr<ID3D11InputLayout> Particle::m_particleLayoutPtr = nullptr;
