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
	void setPowerArrowPos(Map *map);
	void setReady(Map *map);
	void drawUpgradeText(Map *map);

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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_readyTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_button2p;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_button3p;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_button4p;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_buttonQuit;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFontLarger;
	DirectX::SimpleMath::Vector2 catPos;
	DirectX::SimpleMath::Vector2 m_origin;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	DirectX::XMFLOAT2 arrowPos[5];
	std::wstring eStrings[5];
	DirectX::XMFLOAT2 eTextPos[4][5];
	DirectX::XMVECTOR eTextColor[5];

	bool ready[4];

	std::wstring uStrings[8][5][4];
	DirectX::XMFLOAT2 textPos[4][5];

	SimpleMath::Vector2 buttPos[4];
	SimpleMath::Vector2 textOffset;

	DirectX::XMFLOAT2 readyPos;
	
};