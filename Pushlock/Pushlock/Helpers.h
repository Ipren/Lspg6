#pragma once

#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <string>

using namespace DirectX;

class ShaderInclude : public ID3DInclude {
public:
	ShaderInclude(const char* shaderDir, const char* systemDir) : m_ShaderDir(shaderDir), m_SystemDir(systemDir) {}

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	HRESULT __stdcall Close(LPCVOID pData);
private:
	std::string m_ShaderDir;
	std::string m_SystemDir;
};

ID3DBlob *compile_shader(const wchar_t *filename, const char *function, const char *model, ID3D11Device *gDevice);
ID3D11InputLayout *create_input_layout(D3D11_INPUT_ELEMENT_DESC *elements, size_t size, ID3DBlob *blob, ID3D11Device *gDevice);

XMFLOAT4 normalize_color(int32_t color);