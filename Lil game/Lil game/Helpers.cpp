#include "Helpers.h"

#include <stdint.h>
#include <iterator>
#include <fstream>
#include <vector>

#include "dxerr.h"

#include "Globals.h"

// Hjälpfunktion för att kompilera shaders, t.ex:
//
//   ID3D11VertexShader *vertexShader = nullptr;
//
//   ID3DBlob *blob = compile_shader(L"Shader.hlsl", "VS", "vs_5_0");
//   DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));
//
ID3DBlob *compile_shader(const wchar_t *filename, const char *function, const char *model)
{
	ID3DBlob *blob = nullptr;
	ID3DBlob *error = nullptr;

	DXCALL(
		D3DCompileFromFile(
			filename,
			nullptr,
			nullptr,
			function,
			model,
			0,
			0,
			&blob,
			&error)
	);

	if (error) {
		OutputDebugStringA((char*)error->GetBufferPointer());
	}

	return blob;
}

// Hjälpfunktion för att skapa input layouts, t.ex:
//
///  ID3D11VertexShader *vertexShader = nullptr;
//   ID3D11InputLayout *inputLayout = nullptr;
//
//   ID3DBlob *blob = CompileShader(L"Shader.hlsl", "VS", "vs_5_0");
///  DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));
//
///  ...
//
//   D3D11_INPUT_ELEMENT_DESC desc[] = {
//     { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//     { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	 };
//   create_input_layout(desc, ARRAYSIZE(desc), blob, &inputLayout);
//
ID3D11InputLayout *create_input_layout(D3D11_INPUT_ELEMENT_DESC *elements, size_t size, ID3DBlob *blob)
{
	ID3D11InputLayout *layout = nullptr;
	DXCALL(gDevice->CreateInputLayout(elements, size, blob->GetBufferPointer(), blob->GetBufferSize(), &layout));

	return layout;
}