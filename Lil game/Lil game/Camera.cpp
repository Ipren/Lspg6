#include "Camera.h"

Camera::Camera(XMVECTOR pos, XMVECTOR look, ID3D11Device *gDevice)
	: pos(pos), look(look), target({}), temp({}), offset({})
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

void Camera::focus(std::vector<XMVECTOR> positions)
{
	XMVECTOR total = { 0, 0, 0, 0 };

	for (auto pos : positions) {
		total += pos;
	}

	this->target = total / positions.size();
}

void Camera::update(float dt, ID3D11DeviceContext *gDeviceContext)
{
	temp = XMVectorLerp(temp, target, 1.9f * dt);
	offset = XMVectorLerp(offset, temp, 1.9f * dt);

	pos = XMVectorSelect(pos, temp, XMVectorSelectControl(XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0));
	look = XMVectorSelect(look, temp, XMVectorSelectControl(XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0));

	vals.proj = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, WIDTH / (float)HEIGHT, 0.01f, 50.f);
	vals.view = XMMatrixLookAtLH(pos + temp * 0.9, look + temp, { 0, 1, 0 });

	D3D11_MAPPED_SUBRESOURCE data;
	DXCALL(gDeviceContext->Map(wvp_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
	{
		CopyMemory(data.pData, &vals, sizeof(XMMATRIX) * 3);
	}
	gDeviceContext->Unmap(wvp_buffer, 0);
}
