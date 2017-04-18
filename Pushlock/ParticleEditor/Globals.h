#pragma once

#define WIDTH 1600
#define HEIGHT 900

extern IDXGISwapChain *gSwapChain;

extern ID3D11Device *gDevice;
extern ID3D11DeviceContext *gDeviceContext;

extern ID3D11DepthStencilView *gDepthbufferDSV;
extern ID3D11ShaderResourceView *gDepthbufferSRV;
extern ID3D11RenderTargetView *gDepthbufferRTV;
extern ID3D11RenderTargetView *gBackbufferRTV;