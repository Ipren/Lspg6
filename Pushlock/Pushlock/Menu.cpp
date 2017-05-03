#include "Menu.h"
#include "Globals.h"
#include "DirectXTK.h"
#include <DirectXMath.h>

Menu::Menu(Renderer* renderer)
{

	ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(renderer->gDevice, L"cat.dds", &r, &m_texture, 0, nullptr));

	HRESULT hr = DirectX::CreateWICTextureFromFile(renderer->gDevice, renderer->gDeviceContext, L"../Resources/textures/arrow.png ", &r, &this->m_cutexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(renderer->gDevice, renderer->gDeviceContext, L"../Resources/textures/choiceball.png ", &r, &this->m_Balltexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(renderer->gDevice, renderer->gDeviceContext, L"../Resources/textures/readytext.png ", &r, &this->m_readyTexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	r->Release();
	m_spriteBatch = std::make_unique<SpriteBatch>(renderer->gDeviceContext);
	m_spriteFont = std::make_unique<SpriteFont>(renderer->gDevice, L"comicsans.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(renderer->gDeviceContext);

	this->selectedButton = 0;

	/*main menu buttons
	0-start game with 2 players	
	1-start game with 3 players
	2-start game with 4 players
	3-quit*/
	this->buttons.push_back(4);

	//
	//this->buttons[GameState::MainMenu][0] = false;//start game with 2 players

	m_states = std::make_unique<CommonStates>(renderer->gDevice);

	m_effect = std::make_unique<BasicEffect>(renderer->gDevice);
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	renderer->gDevice->CreateInputLayout(VertexPositionColor::InputElements,
			VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			m_inputLayout.ReleaseAndGetAddressOf());

	setSelectedPos(GameState::MainMenu);

}

Menu::~Menu()
{
	m_texture.Reset();
	m_cutexture.Reset();
	m_spriteBatch.reset();

	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
}

#include "imgui.h"

void Menu::render(Renderer* renderer, GameState currentState, int winner, Map *map, int currentRound)
{
	m_spriteBatch->Begin();

	if (currentState != GameState::Playing)
	{
		float clearColor[] = { 0, 0, 0, 1 };

		renderer->gDeviceContext->OMSetRenderTargets(1, &renderer->gBackbufferRTV, nullptr);

		renderer->gDeviceContext->ClearRenderTargetView(renderer->gBackbufferRTV, clearColor);

		UINT vertexSize = sizeof(float) * 5;
		UINT offset = 0;

		renderer->gDeviceContext->IASetVertexBuffers(0, 1, &renderer->quadVertexBuffer, &vertexSize, &offset);
		renderer->gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		renderer->gDeviceContext->IASetInputLayout(renderer->cpQuadLayout);

		renderer->gDeviceContext->VSSetShader(renderer->cpMenuVs, nullptr, 0);
		renderer->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
		renderer->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
		renderer->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
		renderer->gDeviceContext->PSSetShader(renderer->cpmenuPS, nullptr, 0);

		if (currentState == GameState::ChoosePowers)
		{
			
			vertexSize = sizeof(chooseUpgradesVertex);
			renderer->gDeviceContext->IASetVertexBuffers(0, 1, &renderer->cuVertexBuffer, &vertexSize, &offset);
			renderer->gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			renderer->gDeviceContext->IASetInputLayout(renderer->cuLayout);

			renderer->gDeviceContext->VSSetShader(renderer->cuVS, nullptr, 0);
			renderer->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
			renderer->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
			renderer->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
			renderer->gDeviceContext->PSSetShader(renderer->cuPS, nullptr, 0);
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->cpMenuTexture);

			for (size_t i = 1; i < 5; i++)
			{
				renderer->gDeviceContext->PSSetShaderResources(i, 1, &renderer->cpMenuTexture);
			}
			renderer->gDeviceContext->Draw(24, 0);
			this->setPowerArrowPos(map);
		}
		else if (currentState == GameState::MainMenu)
		{
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->mainMenuTexture);
			renderer->gDeviceContext->Draw(6, 0);
		}
		else if (currentState == GameState::EndRound)
		{
			vertexSize = sizeof(chooseUpgradesVertex);
			renderer->gDeviceContext->IASetVertexBuffers(0, 1, &renderer->cuVertexBuffer, &vertexSize, &offset);
			renderer->gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			renderer->gDeviceContext->IASetInputLayout(renderer->cuLayout);
			
			renderer->gDeviceContext->VSSetShader(renderer->cuVS, nullptr, 0);
			renderer->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
			renderer->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
			renderer->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
			renderer->gDeviceContext->PSSetShader(renderer->cuPS, nullptr, 0);
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->cuMenuTexture);
			

			for (size_t i = 1; i < 5; i++)
			{
				renderer->gDeviceContext->PSSetShaderResources(i, 1, &renderer->cuMenuTexture);
			}
			if (currentRound == 0)
			{
				for (size_t i = 0; i < map->nrOfPlayers; i++)
				{
					renderer->gDeviceContext->PSSetShaderResources((i + 1), 1, &renderer->r1CUTextures[map->playerElemnts[i]]);
				}
			}
			renderer->gDeviceContext->Draw(24, 0);
			this->setUpgradesArrowPos(map);

		}
		else if (currentState == GameState::EndGame)
		{
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->endMenuTexture);
			renderer->gDeviceContext->Draw(6, 0);
		}
		

	}
	if (currentState != GameState::MainMenu && currentState != GameState::EndGame)
	{
		renderer->gDeviceContext->OMSetRenderTargets(1, &renderer->gBackbufferRTV, nullptr);

		UINT vertexSize = sizeof(float) * 5;
		UINT offset = 0;

		renderer->gDeviceContext->IASetVertexBuffers(0, 1, &renderer->roundVertexBuffer, &vertexSize, &offset);
		renderer->gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		renderer->gDeviceContext->IASetInputLayout(renderer->cpQuadLayout);

		renderer->gDeviceContext->VSSetShader(renderer->cpMenuVs, nullptr, 0);
		renderer->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
		renderer->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
		renderer->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
		renderer->gDeviceContext->PSSetShader(renderer->cpmenuPS, nullptr, 0);

		renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->scoreBoardTexture);
		renderer->gDeviceContext->Draw(6, 0);

		m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Round: ") + std::to_wstring(map->round)).c_str(), XMFLOAT2(600, 10), Colors::Black);

	}

	
	if (currentState == GameState::MainMenu)
		m_spriteBatch->Draw(m_cutexture.Get(), catPos, nullptr, Colors::White, 0.f, m_origin);
	else if (currentState == GameState::EndGame)
	{
		/*std::wstring s = L"Player: " + (winner + 1);
		wchar_t* c = new wchar_t(&s.c_str());*/
		m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player: ") + std::to_wstring(winner+1)).c_str(), XMFLOAT2(375, 300), Colors::HotPink);
	}
	else if (currentState == GameState::EndRound)
	{
		for (size_t i = 0; i < 4; i++)
		{
			m_spriteBatch->Draw(m_Balltexture.Get(), this->arrowPos[i], nullptr, Colors::White, 0.f, m_origin);
		}
		this->setReady(map);
	}
	else if (currentState == GameState::ChoosePowers)
	{
		for (size_t i = 0; i < 4; i++)
		{
			m_spriteBatch->Draw(m_Balltexture.Get(), this->arrowPos[i], nullptr, Colors::White, 0.f, m_origin);
		}
		this->setReady(map);
	}
	auto pos = ImGui::GetIO();// .MousePos();
	m_spriteFont->DrawString(m_spriteBatch.get(), L"Detta ar en font", XMFLOAT2(pos.MousePos.x, pos.MousePos.y), Colors::HotPink);

	m_spriteBatch->End();

	//renderer->gDeviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	//renderer->gDeviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	//renderer->gDeviceContext->RSSetState(m_states->CullNone());

	//m_effect->Apply(renderer->gDeviceContext);

	//renderer->gDeviceContext->IASetInputLayout(m_inputLayout.Get());


}

void Menu::selectDown(GameState currentState)
{
	if (this->buttons[currentState] - 1 > selectedButton)
	{
		this->selectedButton++;
	}
	else
	{
		selectedButton = 0;
	}
	setSelectedPos(currentState);
}

void Menu::selectUp(GameState currentState)
{
	if (0 < selectedButton)
	{
		selectedButton--;
	}
	else
	{
		selectedButton = buttons[currentState]-1;
	}
	setSelectedPos(currentState);
}

void Menu::setUpgradesArrowPos(Map * map)
{
	float offsetX = 640.0f;
	float offsetY = 400.0f;
	//p1
	if (map->upgradeChoice[0] == 0)
	{
		this->arrowPos[0].y = 293.0f/2;
		this->arrowPos[0].x = 180.0f/2;
	}
	if (map->upgradeChoice[0] == 1)
	{
		this->arrowPos[0].y = 389.0f / 2;
		this->arrowPos[0].x = 180.0f / 2;
	}
	if (map->upgradeChoice[0] == 2)
	{
		this->arrowPos[0].y = 480.0f / 2;
		this->arrowPos[0].x = 180.0f / 2;
	}
	if (map->upgradeChoice[0] == 3)
	{
		this->arrowPos[0].y = 579.0f / 2;
		this->arrowPos[0].x = 180.0f / 2;
	}

	//p2
	if (map->upgradeChoice[1] == 0)
	{
		this->arrowPos[1].y = 293.0f / 2 + offsetY;
		this->arrowPos[1].x = 180.0f / 2;
	}
	if (map->upgradeChoice[1] == 1)
	{
		this->arrowPos[1].y = 389.0f / 2 + offsetY;
		this->arrowPos[1].x = 180.0f / 2;
	}
	if (map->upgradeChoice[1] == 2)
	{
		this->arrowPos[1].y = 480.0f / 2 + offsetY;
		this->arrowPos[1].x = 180.0f / 2;
	}
	if (map->upgradeChoice[1] == 3)
	{
		this->arrowPos[1].y = 579.0f / 2 + offsetY;
		this->arrowPos[1].x = 180.0f / 2;
	}

	//p3
	if (map->upgradeChoice[2] == 0)
	{
		this->arrowPos[2].y = 293.0f / 2;
		this->arrowPos[2].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[2] == 1)
	{
		this->arrowPos[2].y = 389.0f / 2;
		this->arrowPos[2].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[2] == 2)
	{
		this->arrowPos[2].y = 480.0f / 2;
		this->arrowPos[2].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[2] == 3)
	{
		this->arrowPos[2].y = 579.0f / 2;
		this->arrowPos[2].x = 180.0f / 2 + offsetX;
	}

	//p4
	if (map->upgradeChoice[3] == 0)
	{
		this->arrowPos[3].y = 293.0f / 2 + offsetY;
		this->arrowPos[3].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[3] == 1)
	{
		this->arrowPos[3].y = 389.0f / 2 + offsetY;
		this->arrowPos[3].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[3] == 2)
	{
		this->arrowPos[3].y = 480.0f / 2 + offsetY;
		this->arrowPos[3].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[3] == 3)
	{
		this->arrowPos[3].y = 579.0f / 2 + offsetY;
		this->arrowPos[3].x = 180.0f / 2 + offsetX;
	}
}

void Menu::setPowerArrowPos(Map * map)
{
	float offsetX = 640.0f;
	float offsetY = 400.0f;

	//player 1
	if (map->playerElemnts[0] == 0)
	{
		this->arrowPos[0].x = 100 / 2;
		this->arrowPos[0].y = 200 / 2;
	}
	if (map->playerElemnts[0] == 1)
	{
		this->arrowPos[0].x = 100 / 2;
		this->arrowPos[0].y = 310 / 2;
	}
	if (map->playerElemnts[0] == 2)
	{
		this->arrowPos[0].x = 100 / 2;
		this->arrowPos[0].y = 430 / 2;
	}
	if (map->playerElemnts[0] == 3)
	{
		this->arrowPos[0].x = 100 / 2;
		this->arrowPos[0].y = 550 / 2;
	}
	if (map->playerElemnts[0] == 4)
	{
		this->arrowPos[0].x = 100 / 2;
		this->arrowPos[0].y = 670 / 2;
	}

	//player 2
	if (map->playerElemnts[1] == 0)
	{
		this->arrowPos[1].x = 100 / 2 + offsetX;
		this->arrowPos[1].y = 200 / 2;
	}
	if (map->playerElemnts[1] == 1)
	{
		this->arrowPos[1].x = 100 / 2 + offsetX;
		this->arrowPos[1].y = 310 / 2;
	}
	if (map->playerElemnts[1] == 2)
	{
		this->arrowPos[1].x = 100 / 2 + offsetX;
		this->arrowPos[1].y = 430 / 2;
	}
	if (map->playerElemnts[1] == 3)
	{
		this->arrowPos[1].x = 100 / 2 + offsetX;
		this->arrowPos[1].y = 550 / 2;
	}
	if (map->playerElemnts[1] == 4)
	{
		this->arrowPos[1].x = 100 / 2 + offsetX;
		this->arrowPos[1].y = 670 / 2;
	}

	//player 3
	if (map->playerElemnts[2] == 0)
	{
		this->arrowPos[2].x = 100 / 2;
		this->arrowPos[2].y = 200 / 2 + offsetY;
	}
	if (map->playerElemnts[2] == 1)
	{
		this->arrowPos[2].x = 100 / 2;
		this->arrowPos[2].y = 310 / 2 + offsetY;
	}
	if (map->playerElemnts[2] == 2)
	{
		this->arrowPos[2].x = 100 / 2;
		this->arrowPos[2].y = 430 / 2 + offsetY;
	}
	if (map->playerElemnts[2] == 3)
	{
		this->arrowPos[2].x = 100 / 2;
		this->arrowPos[2].y = 550 / 2 + offsetY;
	}
	if (map->playerElemnts[2] == 4)
	{
		this->arrowPos[2].x = 100 / 2;
		this->arrowPos[2].y = 670 / 2 + offsetY;
	}

	//player 4
	if (map->playerElemnts[3] == 0)
	{
		this->arrowPos[3].x = 100 / 2 + offsetX;
		this->arrowPos[3].y = 200 / 2 + offsetY;
	}
	if (map->playerElemnts[3] == 1)
	{
		this->arrowPos[3].x = 100 / 2 + offsetX;
		this->arrowPos[3].y = 310 / 2 + offsetY;
	}
	if (map->playerElemnts[3] == 2)
	{
		this->arrowPos[3].x = 100 / 2 + offsetX;
		this->arrowPos[3].y = 430 / 2 + offsetY;
	}
	if (map->playerElemnts[3] == 3)
	{
		this->arrowPos[3].x = 100 / 2 + offsetX;
		this->arrowPos[3].y = 550 / 2 + offsetY;
	}
	if (map->playerElemnts[3] == 4)
	{
		this->arrowPos[3].x = 100 / 2 + offsetX;
		this->arrowPos[3].y = 670 / 2 + offsetY;
	}

}

void Menu::setReady(Map * map)
{
	float offsetX = 640.0f;
	float offsetY = 400.0f;
	for (size_t i = 0; i < 4; i++)
	{
		this->ready[i] = true;
	}
	int n = 0;
	for (size_t i = 0; i < map->entitys.size(); i++)
	{
		if (dynamic_cast<Player *>(map->entitys[i]) != nullptr)
		{
			this->ready[n] = dynamic_cast<Player *>(map->entitys[i])->ready;
			n++;
		}
		
	}

	if (this->ready[0])
	{
		m_spriteBatch->Draw(m_readyTexture.Get(), DirectX::XMFLOAT2(0.0f, 0.0f), nullptr, Colors::White, 0.f, m_origin);
	}
	if (this->ready[1])
	{
		m_spriteBatch->Draw(m_readyTexture.Get(), DirectX::XMFLOAT2(0.0f + offsetX, 0.0f), nullptr, Colors::White, 0.f, m_origin);
	}
	if (this->ready[2])
	{
		m_spriteBatch->Draw(m_readyTexture.Get(), DirectX::XMFLOAT2(0.0f, 0.0f + offsetY), nullptr, Colors::White, 0.f, m_origin);
	}
	if (this->ready[3])
	{
		m_spriteBatch->Draw(m_readyTexture.Get(), DirectX::XMFLOAT2(0.0f + offsetX, 0.0f + offsetY), nullptr, Colors::White, 0.f, m_origin);
	}


}

void Menu::setSelectedPos(GameState currentState)
{
	if (currentState == GameState::MainMenu)
	{
		if (selectedButton == 0)
		{
			catPos.x = 375;
			catPos.y = 200;
		}
		else if (selectedButton == 1) {
			catPos.x = 375;
			catPos.y = 350;
		}
		else if (selectedButton == 2) {
			catPos.x = 375;
			catPos.y = 500;
		}
		else if (selectedButton == 3) {
			catPos.x = 375;
			catPos.y = 650;
		}
	}
}
