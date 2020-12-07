#pragma once
#include"../Buffer.h"
#include"../ConstantBufferTypes.h"
#include"../VertexStructs.h"
#include"../Timer.h"
#include"../Camera.h"
#include"../Layouts.h"
#include"../CompileShaderUtility.h"
#include"../ResourceHandler.h"
#include"../ShaderProgram.h"
#include"../Transform.h"

enum class ParticleEffect
{
	RAININGDOG,
	SCOREPICKUP,
	PLAYERLINES,
	LINEARRAY,
};

class Particle
{
private:
	static Microsoft::WRL::ComPtr<ID3D11InputLayout> m_particleLayoutPtr;
	static std::map<std::wstring,ID3D11VertexShader*> m_vsMap;
	static std::map<std::wstring,ID3D11GeometryShader*> m_gsMap;
	static std::map<std::wstring,ID3D11PixelShader*> m_psMap;
	static Microsoft::WRL::ComPtr<ID3DBlob> m_vertexShaderBlob;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_resourceViewRandomTextureArray;
	static Microsoft::WRL::ComPtr<ID3D11SamplerState> m_linearSamplerStatePtr;

	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_noDepthNoWriteStencilState; //NO depth test no depth write
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthNoWriteStencilState; //Depth Test no depth write


	Buffer<ParticleDataVS> m_particleDataCB;
	ParticleDataVS m_particleData;
	Buffer<acceleration> m_accelerationDataCB; //RainSpecific
	acceleration m_accelerationData;
	Buffer<projectionMatrix> m_projectionMatrixCB;
	projectionMatrix m_viewProjData;

	ID3D11Buffer* m_vertexBufferRaw;
	ID3D11Buffer* m_streamOutBufferRaw;
	ID3D11Buffer* m_initBufferRaw;

	Transform* m_transform;

	ID3DBlob* m_shaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_resourceViewTexture;

	//Shaders SteamOut
	ID3D11VertexShader* m_streamOutVertexShaderPtr;
	ID3D11GeometryShader* m_streamOutGeometryShaderPtr;

	//Shaders draw
	ID3D11VertexShader* m_drawVertexShaderPtr;
	ID3D11GeometryShader* m_drawGeometryShaderPtr;
	ID3D11PixelShader* m_drawPixelShaderPtr;


	float m_time;
	bool m_doneFirstEmitter;


	Camera* m_camera;

	ParticleVertex m_init;

	void defaultConstructor()
	{
		ZeroMemory(&m_particleData, sizeof(ParticleDataVS));
		ZeroMemory(&m_accelerationData, sizeof(acceleration));
		ZeroMemory(&m_viewProjData, sizeof(projectionMatrix));
		ZeroMemory(&m_init, sizeof(ParticleVertex));

		m_resourceViewTexture = nullptr;
		m_streamOutVertexShaderPtr = nullptr;
		m_streamOutGeometryShaderPtr = nullptr;
		m_drawVertexShaderPtr = nullptr;
		m_drawGeometryShaderPtr = nullptr;
		m_drawPixelShaderPtr = nullptr;

		m_camera = nullptr;

		m_maxNrOfParticles = 0;
		m_time = 0.f;
		m_doneFirstEmitter = false;
		m_neverDie = true;
	}

	virtual void setupCBStreamOutPass(ID3D11DeviceContext* deviceContext) = 0;
	virtual void setupCBDrawPass(ID3D11DeviceContext* deviceContext) = 0;
	virtual void ownEndOfDraw(ID3D11DeviceContext* deviceContext) = 0;


	ID3D11GeometryShader* getGSShader(bool streamout, std::wstring name)
	{
		HRESULT hr = 0;
		if (streamout)
		{
			if (m_gsMap.find(name) != m_gsMap.end())
			{
				return m_gsMap[name];
			}
			else
			{
				const D3D11_SO_DECLARATION_ENTRY declarationEntryArray[] =
				{
					{ 0,"POSITION", 0, 0 , 3, 0 },
					{ 0,"OLDPOS", 0, 0, 3, 0 },
					{ 0,"VEL", 0, 0, 3, 0 },
					{ 0,"SIZE", 0, 0, 2, 0 },
					{ 0,"TIME", 0, 0, 1, 0 },
					{ 0,"OLDTIME", 0, 0, 1, 0 },
					{ 0,"TYPE", 0, 0, 1, 0 }
				};

				hr = compileShader(name.c_str(), "main", "gs_5_0", &this->m_shaderBlob);

				assert(SUCCEEDED(hr));

				hr = m_device->CreateGeometryShaderWithStreamOutput(this->m_shaderBlob->GetBufferPointer(), this->m_shaderBlob->GetBufferSize(),
					declarationEntryArray, _countof(declarationEntryArray), nullptr, 0, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, &m_gsMap[name]);

				assert(SUCCEEDED(hr));

				this->m_shaderBlob->Release();
				return m_gsMap[name];
			}
		}
		else
		{
			if (m_gsMap.find(name) != m_gsMap.end())
			{
				return m_gsMap[name];
			}
			else
			{
				HRESULT hr = compileShader(name.c_str(), "main", "gs_5_0", &this->m_shaderBlob);

				assert(SUCCEEDED(hr));


				hr = m_device->CreateGeometryShader(this->m_shaderBlob->GetBufferPointer(), this->m_shaderBlob->GetBufferSize(), NULL, &m_gsMap[name]);

				assert(SUCCEEDED(hr));

				this->m_shaderBlob->Release();
				return m_gsMap[name];
			}
		}
	}

	ID3D11PixelShader* getPixelShader(std::wstring name)
	{
		if (m_psMap.find(name) != m_psMap.end())
		{
			return m_psMap[name];
		}
		else
		{
			HRESULT hr = compileShader(name.c_str(), "main", "ps_5_0", &this->m_shaderBlob);

			assert(SUCCEEDED(hr));


			hr = m_device->CreatePixelShader(this->m_shaderBlob->GetBufferPointer(), this->m_shaderBlob->GetBufferSize(), NULL, &m_psMap[name]);

			assert(SUCCEEDED(hr));

			this->m_shaderBlob->Release();
			return m_psMap[name];
		}
	}

	ID3D11VertexShader* getVertexShader(std::wstring name)
	{
		if (m_vsMap.find(name) != m_vsMap.end())
		{
			return m_vsMap[name];
		}
		else
		{
			HRESULT hr = compileShader(name.c_str(), "main", "vs_5_0", &this->m_shaderBlob);
			assert(SUCCEEDED(hr));

			hr = m_device->CreateVertexShader(this->m_shaderBlob->GetBufferPointer(), this->m_shaderBlob->GetBufferSize(), NULL, &m_vsMap[name]);
			assert(SUCCEEDED(hr));

			this->m_shaderBlob->Release();
			return m_vsMap[name];
		}
	}

protected:
	std::wstring m_streamOutVertexShader;
	std::wstring m_streamOutGS;
	std::wstring m_drawVertexShader;
	std::wstring m_drawGSShader;
	std::wstring m_drawPSPixel;

	bool m_neverDie;
	int m_maxNrOfParticles;
	ParticleEffect m_particleType;
	std::wstring m_textureName = L"";

	float m_particleSystemLifeTime;
	ID3D11Device* m_device;


public:
	Particle()
	{
		defaultConstructor();
	}
	~Particle()
	{
		m_vertexBufferRaw->Release();
		m_streamOutBufferRaw->Release();
		m_initBufferRaw->Release();

		m_particleDataCB.release();
		m_accelerationDataCB.release();
		m_projectionMatrixCB.release();

	}

	Particle(ParticleEffect particleType, bool neverDie = true)
	{
		defaultConstructor();
		m_particleType = particleType;
		m_neverDie = neverDie;

	}

	static void setupStaticDataForParticle(ID3D11Device* device)
	{
		HRESULT hr = compileShader(L"Particles/StreamOutVertexShader.hlsl", "main", "vs_5_0", m_vertexShaderBlob.GetAddressOf());

		assert(SUCCEEDED(hr));


		hr = device->CreateInputLayout(Layouts::particleLayout, //VertexLayout
			ARRAYSIZE(Layouts::particleLayout), //Nr of elements 
			m_vertexShaderBlob->GetBufferPointer(),
			m_vertexShaderBlob->GetBufferSize(), //Bytecode length
			m_particleLayoutPtr.GetAddressOf()
		);

		setupRandomVectorArray(device);

		D3D11_SAMPLER_DESC samplerStateDesc;
		ZeroMemory(&samplerStateDesc, sizeof(D3D11_SAMPLER_DESC));
		samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerStateDesc.MipLODBias = 0.0f;
		samplerStateDesc.MaxAnisotropy = 1;
		samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerStateDesc.MinLOD = 0;
		samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = device->CreateSamplerState(&samplerStateDesc, m_linearSamplerStatePtr.GetAddressOf());


		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		hr = device->CreateDepthStencilState(&depthStencilDesc, m_noDepthNoWriteStencilState.GetAddressOf());

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		hr = device->CreateDepthStencilState(&depthStencilDesc, m_DepthNoWriteStencilState.GetAddressOf());

		assert(SUCCEEDED(hr));
	}

	static void cleanStaticDataForParticles()
	{
		delete *m_vertexShaderBlob.ReleaseAndGetAddressOf();
		delete *m_particleLayoutPtr.ReleaseAndGetAddressOf();
		delete *m_resourceViewRandomTextureArray.ReleaseAndGetAddressOf();

		delete *m_linearSamplerStatePtr.ReleaseAndGetAddressOf();
		delete *m_noDepthNoWriteStencilState.ReleaseAndGetAddressOf();
		delete *m_DepthNoWriteStencilState.ReleaseAndGetAddressOf();


		std::for_each(m_vsMap.begin(), m_vsMap.end(),
			[](std::pair<std::wstring, ID3D11VertexShader*> element) {

			 SAFE_RELEASE(element.second);
			
		});
		std::for_each(m_gsMap.begin(), m_gsMap.end(),
			[](std::pair<std::wstring, ID3D11GeometryShader*> element) {

			SAFE_RELEASE(element.second);

		});
		std::for_each(m_psMap.begin(), m_psMap.end(),
			[](std::pair<std::wstring, ID3D11PixelShader*> element) {

			SAFE_RELEASE(element.second);

		});

	}

	void setShaders(std::wstring streamOutVertexShader, std::wstring streamOutGS, std::wstring drawVertexShader, std::wstring drawGSShader, std::wstring drawPSPixel, bool doLifeCycklePass = true)
	{
		m_streamOutVertexShader = streamOutVertexShader;
		m_streamOutGS = streamOutGS;
		m_drawVertexShader = drawVertexShader;
		m_drawGSShader = drawGSShader;
		m_drawPSPixel = drawPSPixel;
	}

	void changeTransform(Transform* transform)
	{
		this->m_transform = transform;
	}

	bool initParticle(ID3D11Device* device, Camera* camera, Transform* transform)
	{
		HRESULT hr;
		ResourceHandler* resourceHandler = &ResourceHandler::get();
		this->m_resourceViewTexture = resourceHandler->loadTexture(m_textureName.c_str())->view;
		this->m_camera = camera;
		m_transform = transform;
		m_device = device;

		m_accelerationData.worldAcceleration = { -15.0f, -1.8f, 0.0f };
		m_particleData.intensity = 100.f;


		m_init = ParticleVertex(XMFLOAT3(666, 666, 666), XMFLOAT3(5, 5, 5), XMFLOAT3(0, 0, 0), XMFLOAT2(1, 1), 0.f, (unsigned int)ParticleType::Emitter);
		m_particleDataCB.initializeBuffer(device, true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &m_particleData, 1);
		m_accelerationDataCB.initializeBuffer(device, true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &m_accelerationData, 1);
		m_projectionMatrixCB.initializeBuffer(device, true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &m_viewProjData, 1);


		//GS steamout
		m_streamOutGeometryShaderPtr = this->getGSShader(true, this->m_streamOutGS.c_str());
		//Draw pixel shader
		m_drawPixelShaderPtr = this->getPixelShader(this->m_drawPSPixel.c_str());
		//Draw Vertex Shader
		m_drawVertexShaderPtr = this->getVertexShader(this->m_drawVertexShader.c_str());
		//Draw GS 
		m_drawGeometryShaderPtr = this->getGSShader(false, this->m_drawGSShader.c_str());

		//Steream out vertexshader
		m_streamOutVertexShaderPtr = this->getVertexShader(this->m_streamOutVertexShader.c_str());


		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.ByteWidth = sizeof(ParticleVertex) * 1;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;


		ParticleVertex p;
		ZeroMemory(&p, sizeof(ParticleVertex));
		p.time = 0.0f;
		p.type = 1;

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &p;

		hr = device->CreateBuffer(&vbd, &vinitData, &m_initBufferRaw);


		vbd.ByteWidth = sizeof(ParticleVertex) * m_maxNrOfParticles;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

		hr = device->CreateBuffer(&vbd, 0, &m_vertexBufferRaw);
		hr = device->CreateBuffer(&vbd, 0, &this->m_streamOutBufferRaw);

		return SUCCEEDED(hr);
	}
	void update(const float& dt, const float& gameTime)
	{
		m_time += dt;
		this->m_particleData.dt = dt;
		this->m_particleData.gameTime = gameTime;
		this->m_particleData.worldEmitPosition = m_transform->getTranslation();

		this->m_viewProjData.g_viewProjectionMatrix = DirectX::XMMatrixTranspose(XMMatrixIdentity() *  this->m_camera->getViewMatrix() * this->m_camera->getProjectionMatrix());
		this->m_viewProjData.g_cameraPos = this->m_camera->getFloat3Position();
	}

	//Entire shader in pipeline should be null before this function is called and the particle layout should be set.
	bool draw(ID3D11DeviceContext* dContext)
	{
		UINT offset = 0;
		UINT stride = sizeof(ParticleVertex);

		m_accelerationData.worldAcceleration = { -1.0f, -4.8f, 0.0f };
		m_accelerationDataCB.updateBuffer(dContext, &m_accelerationData, 1);

		m_projectionMatrixCB.updateBuffer(dContext, &m_viewProjData, 1);
		this->m_particleDataCB.updateBuffer(dContext, &this->m_particleData, 1);

		dContext->IASetInputLayout(this->m_particleLayoutPtr.Get());
		dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		dContext->GSSetSamplers(0, 1, this->m_linearSamplerStatePtr.GetAddressOf());


		//UpdatePhase/Streamout
		dContext->VSSetShader(m_streamOutVertexShaderPtr, nullptr, 0);
		dContext->GSSetShader(m_streamOutGeometryShaderPtr, nullptr, 0);
		dContext->GSSetShaderResources(0, 1, Particle::m_resourceViewRandomTextureArray.GetAddressOf());
		dContext->GSSetConstantBuffers(0, 1, this->m_particleDataCB.GetAddressOf());
		this->setupCBStreamOutPass(dContext);

		if (m_doneFirstEmitter)
		{
			dContext->IASetVertexBuffers(0, 1, &m_vertexBufferRaw, &stride, &offset);

		}
		else
		{
			dContext->IASetVertexBuffers(0, 1, &m_initBufferRaw, &stride, &offset);
		}

		//Set streamout
		dContext->SOSetTargets(1, &this->m_streamOutBufferRaw, &offset);
		//Disable depth write
		dContext->OMSetDepthStencilState(m_noDepthNoWriteStencilState.Get(), 0);


		if (m_doneFirstEmitter)
		{
			dContext->DrawAuto();
		}
		else
		{
			dContext->Draw(1, 0);
			m_doneFirstEmitter = true;
		}

		//Draw state
		ID3D11Buffer* buffer[1] = { 0 };
		dContext->SOSetTargets(1, buffer, &offset); //Unbind steamout target 

		std::swap(this->m_vertexBufferRaw, this->m_streamOutBufferRaw);
		
		this->setupCBDrawPass(dContext);
		//Draw pass
		dContext->IASetVertexBuffers(0, 1, &this->m_vertexBufferRaw, &stride, &offset);
		//Change to draw
		dContext->VSSetShader(m_drawVertexShaderPtr, nullptr, 0);
		dContext->GSSetShader(m_drawGeometryShaderPtr, nullptr, 0);
		dContext->PSSetShader(m_drawPixelShaderPtr, nullptr, 0);

		dContext->OMSetDepthStencilState(m_DepthNoWriteStencilState.Get(), 0);
		dContext->PSSetSamplers(0, 1, this->m_linearSamplerStatePtr.GetAddressOf());
		dContext->PSSetShaderResources(0, 1, this->m_resourceViewTexture.GetAddressOf());
		dContext->VSSetConstantBuffers(0, 1, this->m_accelerationDataCB.GetAddressOf());
		dContext->GSSetConstantBuffers(0, 1, this->m_accelerationDataCB.GetAddressOf());
		dContext->GSSetConstantBuffers(1, 1, this->m_projectionMatrixCB.GetAddressOf());

		dContext->DrawAuto();
		//"Parent" draw function needs to set pipeline and depthstate to wished functionality after this draw function.

		this->ownEndOfDraw(dContext);

		bool shouldDie = false;
		if (!m_neverDie)
		{
			if (m_time > m_particleSystemLifeTime)
			{
				shouldDie = true;
			}
		}
		return shouldDie;
	}

	void restart()
	{
		this->m_doneFirstEmitter = false;
		this->m_time = 0.f;
	}


	static float randomFloat()
	{
		float r = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f;
		return r;
	}

	static void setupRandomVectorArray(ID3D11Device* device)
	{
		const int ARRAYSIZE = 640;
		XMFLOAT4 randomArray[ARRAYSIZE];
		ID3D11Texture1D* randomTexture = nullptr;

		for (int i = 0; i < ARRAYSIZE; i++)
		{

			randomArray[i].x = randomFloat() - 1;
			randomArray[i].y = randomFloat() - 1;
			randomArray[i].z = randomFloat() - 1;
			randomArray[i].w = randomFloat() - 1;
		}

		D3D11_TEXTURE1D_DESC randomTextureDesc;
		randomTextureDesc.ArraySize = 1;
		randomTextureDesc.Width = 640;
		randomTextureDesc.MipLevels = 1;
		randomTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		randomTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		randomTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		randomTextureDesc.CPUAccessFlags = 0;
		randomTextureDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA randomData;
		ZeroMemory(&randomData, sizeof(D3D11_SUBRESOURCE_DATA));
		randomData.SysMemPitch = ARRAYSIZE * sizeof(XMFLOAT4);
		randomData.pSysMem = randomArray;

		HRESULT hr = device->CreateTexture1D(&randomTextureDesc, &randomData, &randomTexture);

		if (FAILED(hr))
			ErrorLogger::get().logError("failed to create random texture for particles!");

		D3D11_SHADER_RESOURCE_VIEW_DESC randomViewTextureDesc;
		ZeroMemory(&randomViewTextureDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		randomViewTextureDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		randomViewTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		randomViewTextureDesc.Texture1D.MipLevels = 1;

		hr = device->CreateShaderResourceView(randomTexture, &randomViewTextureDesc, Particle::m_resourceViewRandomTextureArray.GetAddressOf());
		
		if (FAILED(hr))
			ErrorLogger::get().logError("failed to create random texture shader resource view for particles!");

		randomTexture->Release();
		randomTexture = nullptr;
	}
};