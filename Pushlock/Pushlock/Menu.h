#pragma once
#include <DirectXMath.h>
#include "DirectXTK.h"
#include "Renderer.h"


class Menu
{
public:
	Menu(Renderer* renderer);
	virtual ~Menu();

	void render(Renderer* renderer);
	void addQuad(XMFLOAT2 pos1, XMFLOAT2 pos2, XMFLOAT2 pos3, XMFLOAT2 pos4,
		XMFLOAT3 color1, XMFLOAT3 color2, XMFLOAT3 color3, XMFLOAT3 color4);

	void addButton(std::string name, XMFLOAT2 pos1, XMFLOAT2 pos2, XMFLOAT2 pos3, XMFLOAT2 pos4,
		XMFLOAT3 color1, XMFLOAT3 color2, XMFLOAT3 color3, XMFLOAT3 color4);

private:
	struct Quad
	{
		XMFLOAT2 pos[4];
		XMFLOAT3 color[4];
	};

	std::vector<Quad> buttons;
	std::vector<Quad> quads;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	DirectX::SimpleMath::Vector2 m_screenPos;
	DirectX::SimpleMath::Vector2 m_origin;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};