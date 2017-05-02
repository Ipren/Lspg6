#pragma once
#include <DirectXMath.h>
#include "DirectXTK.h"
#include "Renderer.h"


class Menu
{
public:
	Menu(Renderer* renderer);
	virtual ~Menu();

	void render(Renderer* renderer, GameState currentState, int winner, Map *map, int currentRound);

	std::vector<int> buttons;
	void selectDown(GameState currentState);
	void selectUp(GameState currentState);
	int getSelectedButton()const { return this->selectedButton; }
	void setUpgradesArrowPos(Map *map);
private:
	struct mQuad
	{
		XMFLOAT2 pos[4];
		XMFLOAT3 color[4];
	};
	int selectedButton;
	void setSelectedPos(GameState currentState);

	
	std::vector<mQuad> quads;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cutexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Balltexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_winner;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	DirectX::SimpleMath::Vector2 catPos;
	DirectX::SimpleMath::Vector2 m_origin;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	DirectX::XMFLOAT2 arrowPos[4];
	
};