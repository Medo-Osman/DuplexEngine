#include"Particle.h"


Microsoft::WRL::ComPtr<ID3DBlob> Particle::m_vertexShaderBlob = nullptr;
Microsoft::WRL::ComPtr<ID3D11InputLayout> Particle::m_particleLayoutPtr = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Particle::m_resourceViewRandomTextureArray = nullptr;
Microsoft::WRL::ComPtr<ID3D11SamplerState> Particle::m_linearSamplerStatePtr = nullptr;

Microsoft::WRL::ComPtr<ID3D11DepthStencilState> Particle::m_noDepthNoWriteStencilState = nullptr; //NO depth test no depth write
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> Particle::m_DepthNoWriteStencilState = nullptr; //Depth Test no depth write

std::map<std::wstring, ID3D11VertexShader*> Particle::m_vsMap = std::map<std::wstring, ID3D11VertexShader*>();
std::map<std::wstring, ID3D11GeometryShader*> Particle::m_gsMap = std::map<std::wstring, ID3D11GeometryShader*>();
std::map<std::wstring, ID3D11PixelShader*> Particle::m_psMap = std::map<std::wstring, ID3D11PixelShader*>();
