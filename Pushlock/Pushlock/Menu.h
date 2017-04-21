#pragma once
#include <DirectXMath.h>
#include "DirectXTK.h"
#include "Renderer.h"


class Menu
{
public:
	Menu(Renderer* renderer);
	virtual ~Menu();

	void render(Renderer* renderer, GameState currentState);
	void update();


	std::vector<std::vector<bool>> buttons;
	void selectDown(GameState currentState);
	void selectUp(GameState currentState);
	int getSelectedButton()const { return this->selectedButton; }
private:
	struct mQuad
	{
		XMFLOAT2 pos[4];
		XMFLOAT3 color[4];
	};
	int selectedButton;

	
	std::vector<mQuad> quads;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	DirectX::SimpleMath::Vector2 m_screenPos;
	DirectX::SimpleMath::Vector2 m_origin;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	
};