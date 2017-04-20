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

private:
	struct Button
	{
		DirectX::XMVECTOR quad[4];
	};
	DirectX::XMVECTOR menuQuad[4];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	DirectX::SimpleMath::Vector2 m_screenPos;
	DirectX::SimpleMath::Vector2 m_origin;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};