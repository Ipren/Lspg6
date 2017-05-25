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
#include "ParticleSystem.h"

#include "DirectXTK.h"
#include "WICTextureLoader.h"

extern ParticleSystem *FXSystem;

class Renderer
{
public:
	Renderer();
	Renderer(HWND wndHandle, int width, int height);
	virtual ~Renderer();

	void render(Map *map, Camera *camera, float dt);
	void present();
	void update(float dt, Map *map, Camera *camera);

	ID3D11Device *gDevice;
	ID3D11DeviceContext *gDeviceContext;
	ID3D11Buffer *quadVertexBuffer;
	ID3D11Buffer *roundVertexBuffer;
	ID3D11VertexShader *cpMenuVs;
	ID3D11PixelShader *cpmenuPS;
	ID3D11InputLayout *cpQuadLayout;
	ID3D11ShaderResourceView *cpMenuTexture;
	ID3D11ShaderResourceView *mainMenuTexture;
	ID3D11ShaderResourceView *scoreBoardTexture;
	ID3D11ShaderResourceView *cuMenuTexture;
	ID3D11ShaderResourceView *endMenuTexture;
	ID3D11RenderTargetView *gBackbufferRTV;

	ID3D11Buffer *cuVertexBuffer;
	ID3D11InputLayout *cuLayout;
	ID3D11VertexShader *cuVS;
	XMFLOAT3 directionalLightPos;
	XMFLOAT3 directionalLightFocus;

	Mesh *mapmesh;
	ID3D11PixelShader *cuPS;
	ID3D11ShaderResourceView *r1CUTextures[5];

	ID3D11RasterizerState *ShadowRaster;
	ID3D11RasterizerState *DefaultRaster;

	float shadowznear = 1.f;
	float shadowzfar = 30.f;

	ID3D11RenderTargetView *default_rtv;
	ID3D11ShaderResourceView *default_srv;

	ID3D11RenderTargetView *blur_rtv[2];
	ID3D11ShaderResourceView *blur_srv[2];

	ID3D11Buffer *blur_fs_vertices;
	ID3D11VertexShader *blur_fs_vs;
	ID3D11InputLayout *blur_fs_layout;
	ID3D11SamplerState *blur_fs_sampler;

	ID3D11PixelShader *gaussian_x_ps;
	ID3D11PixelShader *gaussian_y_ps;
	ID3D11BlendState *particle_blend;

	ID3D11PixelShader *blur_composite;
private:
	IDXGISwapChain *gSwapChain;
	
	ID3D11DepthStencilState *DepthStateReadWrite;
	ID3D11DepthStencilState *DepthStateRead;
	ID3D11DepthStencilState *DepthStateDisable;

	ID3D11DepthStencilView *DepthBufferMS;
	ID3D11DepthStencilView *DepthBuffer;
	ID3D11ShaderResourceView* DepthBufferSRV;



	ID3D11Buffer *color_buffer;

	//ID3D11Buffer* menu_buffer;
	//ID3D11InputLayout *menu_layout;
	//ID3D11VertexShader *menu_vsh;
	//ID3D11PixelShader *menu_psh;

	ID3D11Buffer *debug_map_quad;
	ID3D11InputLayout *debug_map_layout;
	ID3D11VertexShader *debug_map_vsh;
	ID3D11PixelShader *debug_map_psh;

	ID3D11VertexShader *mesh_vsh;
	ID3D11PixelShader *mesh_psh;

	ID3D11Buffer *debug_entity_circle;
	ID3D11InputLayout *debug_entity_layout;
	ID3D11VertexShader *debug_entity_vsh;
	ID3D11PixelShader *debug_entity_psh;

	ID3D11VertexShader *model_vsh;
	ID3D11PixelShader *modely_psh;

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

	
	
	Camera::BufferVals shadow_camera;
	ID3D11Buffer *shadow_wvp_buffer;
	ID3D11SamplerState *shadowMapSampler;
	ID3D11RenderTargetView *shadowMapRTV;
	ID3D11ShaderResourceView *shadowMapSRV;
	ID3D11VertexShader *shadowMapVS;
	ID3D11PixelShader *shadowMapPS;

	ID3D11Buffer* mapVBuffer;
	ID3D11InputLayout* mapLayout;
	ID3D11Buffer* shrinkBuffer;
	ID3D11VertexShader *MapVS;
	ID3D11PixelShader *MapPS;
	ID3D11ShaderResourceView *mapTexture;
	ID3D11BlendState *mapBlendState;

	ID3D11Buffer* lavaBuffer;
	ID3D11ShaderResourceView *lavaTexture;
	ID3D11VertexShader* lavaVS;
	ID3D11PixelShader* lavaPS;
	ID3D11Buffer* heatHazeBuffer;

	ID3D11UnorderedAccessView* nullUAV;
	ID3D11ShaderResourceView* nullSRV;
	ID3D11RenderTargetView* nullRTV;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	int width; 
	int height;
	int emitterCount;
	float totalTime;
	float lastParticleInsert;
	int pointLightCount;
	float toatlShrunkAmount;
	float heatHazeCounter;
	bool gb;

	//void create_menu();
	void createBlurPass();
	void create_debug_entity();
	void createShadowMap();
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
	void createScoreQuad();
	void loadTexture();
	void createHPBuffers();
	void createHPShaders();
	void createCuBuffers();
	void createCUShaders();

	void createMapResurces();
	
	
	void swapBuffers();

	void updateDTimeBuffer(float dt);
	void updateEmitters(Map *map);
	void updateParticles(float dt, Map *map);
	void shrinkMap(Map *map);
	void updateCameraPosBuffer(Camera *cam);
	void updatePointLights(Map *map);
	void updatecooldownGUI(Player *player);
	void updateHPBuffers(Player *player);
	void updateheatHaze();

	void renderShadowMap(Map *map, Camera *cam);
	void renderBlurPass(Map *map, Camera *cam);

	void renderCooldownGUI(Map *map, Camera *cam);
	void renderHPGUI(Map *map, Camera *cam, float dt);
	void renderParticles(Camera *camera);
	void renderMap(Camera *cam);

	
};
