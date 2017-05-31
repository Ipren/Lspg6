#pragma once
#include "Globals.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include "DirectXTK.h"
#include "WICTextureLoader.h"
#include <fstream>

class Texture
{
public:
	ID3D11ShaderResourceView* texture;
	ID3D11SamplerState* samplerState;
	//file is in relation to project
	Texture(const wchar_t* file)
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);

		std::ifstream poop(file);
		assert(poop.is_open());
		poop.close();

		ID3D11Resource *tex = nullptr;
		HRESULT hr = DirectX::CreateWICTextureFromFile(globalDevice, globalDeviceContext, file, &tex, &this->texture);
		if (FAILED(hr))
			MessageBox(0, L"texture creation failed", L"error", MB_OK);

		/// sampler///
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = 3;
		samplerDesc.MipLODBias = 0;




		globalDevice->CreateSamplerState(&samplerDesc, &this->samplerState);
		/// /////////
	}
	void Bind()
	{
		globalDeviceContext->PSSetShaderResources(2, 1, &this->texture);
		globalDeviceContext->PSSetSamplers(2, 1, &this->samplerState);
	}
	~Texture()
	{
		texture->Release();
	}
};

