#pragma once

#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>

ID3DBlob *compile_shader(const wchar_t *filename, const char *function, const char *model);
ID3D11InputLayout *create_input_layout(D3D11_INPUT_ELEMENT_DESC *elements, size_t size, ID3DBlob *blob);