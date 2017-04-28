#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <cstdio>

#include "dxerr.h"
#include "Globals.h"
#include "Helpers.h"
#include "Camera.h"
#include "Map.h"
#include "Player.h"

#include "DirectXTK.h"
#include "WICTextureLoader.h"

class Renderer
{
public:
	Renderer();
	Renderer(HWND wndHandle, int width, int height);
	virtual ~Renderer();

	void render(Map *map, Camera *camera);
	void present();
	void update(float dt, Map *map);
	
	ID3D11Device *gDevice;
	ID3D11DeviceContext *gDeviceContext;
	ID3D11Buffer *quadVertexBuffer;
	ID3D11VertexShader *cpMenuVs;
	ID3D11PixelShader *cpmenuPS;
	ID3D11InputLayout *cpQuadLayout;
	ID3D11ShaderResourceView *cpMenuTexture;
	ID3D11ShaderResourceView *mainMenuTexture;
	ID3D11ShaderResourceView *cuMenuTexture;
	ID3D11ShaderResourceView *endMenuTexture;
	ID3D11RenderTargetView *gBackbufferRTV;

	ID3D11Buffer *cuVertexBuffer;
	ID3D11InputLayout *cuLayout;
	ID3D11VertexShader *cuVS;
	ID3D11PixelShader *cuPS;
	ID3D11ShaderResourceView *r1CUTextures[5];

private:
	IDXGISwapChain *gSwapChain;
	
	ID3D11DepthStencilState *DepthStateReadWrite;
	ID3D11DepthStencilState *DepthStateRead;
	ID3D11DepthStencilState *DepthStateDisable;

	ID3D11DepthStencilView *gDepthStencil;

	ID3D11Buffer *color_buffer;

	//ID3D11Buffer* menu_buffer;
	//ID3D11InputLayout *menu_layout;
	//ID3D11VertexShader *menu_vsh;
	//ID3D11PixelShader *menu_psh;

	ID3D11Buffer *debug_map_quad;
	ID3D11InputLayout *debug_map_layout;
	ID3D11VertexShader *debug_map_vsh;
	ID3D11PixelShader *debug_map_psh;

	ID3D11Buffer *debug_entity_circle;
	ID3D11InputLayout *debug_entity_layout;
	ID3D11VertexShader *debug_entity_vsh;
	ID3D11PixelShader *debug_entity_psh;

	ID3D11Debug *debugDevice;

	ID3D11UnorderedAccessView *UAVS[2];
	ID3D11ShaderResourceView* SRVS[2];
	ID3D11Buffer *ParticleCount;
	ID3D11Buffer *inderectArgumentBuffer;
	ID3D11ComputeShader *computeShader;
	ID3D11ComputeShader *inserter;
	ID3D11VertexShader* pVertex;
	ID3D11GeometryShader* pGeometry;
	ID3D11PixelShader* pPixel;
	ID3D11Buffer* deltaTimeBuffer;
	ID3D11Buffer* eLocations;
	ID3D11Buffer* emitterCountBuffer;
	ID3D11ShaderResourceView* emitterSRV;
	ID3D11Buffer* randomVecBufer;
	ID3D11ComputeShader* stompInserter;
	ID3D11Buffer *stompParticles;
	ID3D11ShaderResourceView *stompSRV;
	ID3D11Buffer* playerPosBuffer;

	ID3D11ShaderResourceView *particle_srv;
	ID3D11SamplerState *particle_sampler;
	ID3D11BlendState *particle_blend;

	ID3D11Buffer *dLightBuffer;
	ID3D11Buffer *pLightBuffer;
	ID3D11ShaderResourceView *pLightSRV;
	ID3D11Buffer *cameraPosBuffer;
	ID3D11Buffer *pointLightCountBuffer;

	ID3D11Buffer *cooldownBuffer;
	ID3D11Buffer *cooldownCircles;
	ID3D11InputLayout *cooldownCirclesLayout;
	ID3D11VertexShader *cooldownVS;
	ID3D11PixelShader *cooldownPS;

	ID3D11Buffer *HPVertexBuffer;
	ID3D11Buffer *HPBuffer;
	ID3D11InputLayout *HPInputLayout;
	ID3D11VertexShader *HPVS;
	ID3D11PixelShader *HPPS;

	
	

	ID3D11UnorderedAccessView* nullUAV;
	ID3D11ShaderResourceView* nullSRV;
	ID3D11RenderTargetView* nullRTV;

	int width; 
	int height;
	int emitterCount;
	float totalTime;
	float lastParticleInsert;
	int pointLightCount;

	//void create_menu();
	void create_debug_entity();
	void createShaders();
	void createDepthBuffers();
	HRESULT createDirect3DContext(HWND wndHandle);
	void setViewPort(int width, int height);
	void createParticleBuffer(int nrOfParticles);
	void createParticleShaders();
	void createCooldownBuffers();
	void createCooldownShaders();
	void createStompParticles(DirectX::XMFLOAT3 pos, int type);
	void createLightBuffers();
	void createCameraBuffer();
	void createcpMenuShaders();
	void createFullScreenQuad();
	void loadTexture();
	void createHPBuffers();
	void createHPShaders();
	void createCuBuffers();
	void createCUShaders();
	
	
	void swapBuffers();

	void updateDTimeBuffer(float dt);
	void updateEmitters(Map *map);
	void updateParticles(float dt, Map *map);
	void shrinkMap(Map *map);
	void updateCameraPosBuffer(Camera *cam);
	void updatePointLights(Map *map);
	void updatecooldownGUI(Player *player);
	void updateHPBuffers(Player *player);


	void renderCooldownGUI(Map *map, Camera *cam);
	void renderHPGUI(Map *map, Camera *cam);
	void renderParticles(Camera *camera);

	
};
