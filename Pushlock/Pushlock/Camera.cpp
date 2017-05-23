#include "Camera.h"
#include "Constants.h"

Camera::Camera(XMVECTOR pos, XMVECTOR look, ID3D11Device *gDevice)
	: pos(pos), look(look), target({}), temp({}), offset({}), znear(0.1), zfar(500.f)
{
	vals.world = XMMatrixIdentity();
	vals.proj = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, WIDTH / (float)HEIGHT, 0.1f, 500.f);
	vals.view = XMMatrixLookAtLH(pos, look, { 0, 1, 0 });

	vals.view = DirectX::XMMatrixTranspose(vals.view);
	vals.proj = DirectX::XMMatrixTranspose(vals.proj);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(BufferVals);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vals;

	DXCALL(gDevice->CreateBuffer(&desc, &data, &wvp_buffer));


	DirectX::XMMATRIX wvp= XMMatrixIdentity();
	wvp = DirectX::XMMatrixMultiply(vals.view, vals.proj);

	DirectX::XMFLOAT4X4 tw;
	DirectX::XMStoreFloat4x4(&tw, wvp);

	DirectX::XMFLOAT4X4 tv;
	DirectX::XMStoreFloat4x4(&tv, vals.view);

	DirectX::XMFLOAT4X4 tp;
	DirectX::XMStoreFloat4x4(&tp, vals.proj);
	this->wvpmatrixes.world = tw;
	this->wvpmatrixes.world = tv;
	this->wvpmatrixes.world = tp;

	desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * 3;
	data.pSysMem = &tw;

	gDevice->CreateBuffer(&desc, &data, &this->floatwvpBuffer);
	
}

Camera::~Camera()
{
	this->wvp_buffer->Release();
	this->floatwvpBuffer->Release();
}

void Camera::focus(std::vector<XMVECTOR> positions)
{
	XMVECTOR total = { 0, 0, 0, 0 };

	for (auto pos : positions) {
		total += pos;
	}

	this->target = total / (float)max(positions.size(), 1);
}

void Camera::update(float dt, ID3D11DeviceContext *gDeviceContext)
{
	temp = XMVectorLerp(temp, target, gGameConstants.kCameraSpeed * dt);
	offset = XMVectorLerp(offset, temp, gGameConstants.kCameraSpeed * dt);

	pos = XMVectorSelect(pos, temp, XMVectorSelectControl(XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0));
	look = XMVectorSelect(look, temp, XMVectorSelectControl(XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0));

	vals.proj = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, WIDTH / (float)HEIGHT, znear, zfar);
	vals.view = XMMatrixLookAtLH(pos + temp * gGameConstants.kCameraDrag, look + temp, { 0, 1, 0 });
	vals.normal = XMMatrixTranspose(XMMatrixInverse(nullptr, vals.world));

	vals.inverse = XMMatrixInverse(nullptr, XMMatrixMultiply(vals.view, vals.proj));
	
	D3D11_MAPPED_SUBRESOURCE data;
	DXCALL(gDeviceContext->Map(wvp_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
	{
		CopyMemory(data.pData, &vals, sizeof(BufferVals));
	}
	gDeviceContext->Unmap(wvp_buffer, 0);


	DirectX::XMMATRIX tempWorld = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);

	DirectX::XMMATRIX tempView = vals.view;
	tempView = XMMatrixTranspose(vals.view);

	DirectX::XMMATRIX tempProj = vals.proj;
	tempProj = XMMatrixTranspose(vals.proj);

	DirectX::XMFLOAT4X4 tw;
	DirectX::XMStoreFloat4x4(&tw, tempWorld);

	DirectX::XMFLOAT4X4 tv;
	DirectX::XMStoreFloat4x4(&tv, tempView);

	DirectX::XMFLOAT4X4 tp;
	DirectX::XMStoreFloat4x4(&tp, tempProj);


	this->wvpmatrixes.world = tw;
	this->wvpmatrixes.view = tv;
	this->wvpmatrixes.proj = tp;

	gDeviceContext->Map(this->floatwvpBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &this->wvpmatrixes, sizeof(DirectX::XMFLOAT4X4) * 3);
	gDeviceContext->Unmap(this->floatwvpBuffer, 0);
}

void Camera::updateCamPosBuffer(ID3D11Buffer * rBuffer)
{
}
