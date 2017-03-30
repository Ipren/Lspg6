#include "Camera.h"

Camera::Camera(XMVECTOR pos, XMVECTOR look)
	: pos(pos), look(look)
{
	vals.world = XMMatrixIdentity();
	vals.proj = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, WIDTH / (float)HEIGHT, 0.01f, 50.f);
	vals.view = XMMatrixLookAtLH(pos, look, { 0, 1, 0 });

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(XMMATRIX) * 3;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vals;

	DXCALL(gDevice->CreateBuffer(&desc, &data, &wvp_buffer));
}

Camera::~Camera()
{
}

void Camera::update(float dt)
{
	vals.proj = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, WIDTH / (float)HEIGHT, 0.01f, 50.f);
	vals.view = XMMatrixLookAtLH(pos, look, { 0, 1, 0 });

	D3D11_MAPPED_SUBRESOURCE data;
	DXCALL(gDeviceContext->Map(wvp_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
	{
		CopyMemory(data.pData, &vals, sizeof(XMMATRIX) * 3);
	}
	gDeviceContext->Unmap(wvp_buffer, 0);
}
