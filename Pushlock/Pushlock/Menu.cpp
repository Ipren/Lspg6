#include "Menu.h"
#include "Globals.h"
#include "DirectXTK.h"
#include <DirectXMath.h>
#include "Constants.h"

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
	m_spriteFont = std::make_unique<SpriteFont>(renderer->gDevice, L"morpheus.spritefont");
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


	//////////////////////round 1////////////////////////////////////////////
	//arcane
	this->uStrings[0][0][0] = L"X: Spilts the projectile into 3 weaker ones";
	this->uStrings[0][0][1] = L"Y: Turns the dash into a teleport";
	this->uStrings[0][0][2] = L"A: Improves seeking";
	this->uStrings[0][0][3] = L"B: Projectile lives longer";

	//fire
	this->uStrings[0][1][0] = L"X: Gives your projectile a damage over time effect";
	this->uStrings[0][1][1] = L"Y: Resets the cooldown of dash if you hit with a stomp";
	this->uStrings[0][1][2] = L"A: Larger explotion Radius";
	this->uStrings[0][1][3] = L"B: Increase your maximum health";

	//wind
	this->uStrings[0][2][0] = L"X: Refelcts projectiles when dashing";
	this->uStrings[0][2][1] = L"Y: Can dash trough walls";
	this->uStrings[0][2][2] = L"A: Stronger projectiles";
	this->uStrings[0][2][3] = L"B: Shorter dash cooldown";

	//earth
	this->uStrings[0][3][0] = L"X: Stomps in a line infron of you";
	this->uStrings[0][3][1] = L"Y: Stomp creates projectiles";
	this->uStrings[0][3][2] = L"A: You take less pushback";
	this->uStrings[0][3][3] = L"B: Stronger stomp";

	//water
	this->uStrings[0][4][0] = L"X: Projectile hits slow";
	this->uStrings[0][4][1] = L"Y: You can chooose when to split the projectile";
	this->uStrings[0][4][2] = L"A: Stronger projectles";
	this->uStrings[0][4][3] = L"B: Shorter wall cooldown";

	//////////////////////round 2////////////////////////////////////////////
	//arcane
	this->uStrings[1][0][0] = L"X: Can return to the position you teleported from";
	this->uStrings[1][0][1] = L"Y: Teleporting into someone causes knockback";
	this->uStrings[1][0][2] = L"A: Improves seeking";
	this->uStrings[1][0][3] = L"B: Projectile lives longer";

	//fire
	this->uStrings[1][1][0] = L"X: Dash leaves fire trail that applies a DOT";
	this->uStrings[1][1][1] = L"Y: Wall applies a damage over time effect";
	this->uStrings[1][1][2] = L"A: Larger explotion Radius";
	this->uStrings[1][1][3] = L"B: Increase your maximum health";

	//wind
	this->uStrings[1][2][0] = L"X: Stomping creates a cloud that applies a DOT";
	this->uStrings[1][2][1] = L"Y: Stomping leaves a beacon that can stomp again";
	this->uStrings[1][2][2] = L"A: Stronger projectiles";
	this->uStrings[1][2][3] = L"B: Shorter dash cooldown";

	//earth
	this->uStrings[1][3][0] = L"X: Gives your walls spikes that causes damage";
	this->uStrings[1][3][1] = L"Y: Wall attracts nearby entitys";
	this->uStrings[1][3][2] = L"A: You take less pushback";
	this->uStrings[1][3][3] = L"B: Stronger stomp";

	//water
	this->uStrings[1][4][0] = L"X: Dashing causes greater knockback when colliding";
	this->uStrings[1][4][1] = L"Y: Your dash leaves a ice patch after you";
	this->uStrings[1][4][2] = L"A: Stronger projectles";
	this->uStrings[1][4][3] = L"B: Shorter wall cooldown";


	//////////////////////round 3////////////////////////////////////////////
	//arcane
	this->uStrings[2][0][0] = L"X: Shorter projectile cooldown";
	this->uStrings[2][0][1] = L"Y: Higher projectile speed";
	this->uStrings[2][0][2] = L"A: Improves seeking";
	this->uStrings[2][0][3] = L"B: Projectile lives longer";

	//fire
	this->uStrings[2][1][0] = L"X: Stonger projectile";
	this->uStrings[2][1][1] = L"Y: Stronger stomp";
	this->uStrings[2][1][2] = L"A: Larger explotion Radius";
	this->uStrings[2][1][3] = L"B: Increase your maximum health";

	//wind
	this->uStrings[2][2][0] = L"X: Faster projectile";
	this->uStrings[2][2][1] = L"Y: Stronger stomp";
	this->uStrings[2][2][2] = L"A: Stronger projectiles";
	this->uStrings[2][2][3] = L"B: Shorter dash cooldown";

	//earth
	this->uStrings[2][3][0] = L"X: Faster projectile";
	this->uStrings[2][3][1] = L"Y: Larger projectile effect radius";
	this->uStrings[2][3][2] = L"A: You take less pushback";
	this->uStrings[2][3][3] = L"B: Stronger stomp";

	//water
	this->uStrings[2][4][0] = L"X: Faster projectile";
	this->uStrings[2][4][1] = L"Y: Larger stomp distance";
	this->uStrings[2][4][2] = L"A: Stronger projectles";
	this->uStrings[2][4][3] = L"B: Shorter wall cooldown";


	//rest of the rounds
	for (size_t i = 3; i < 8; i++)
	{
		/*for (size_t j = 0; j < 5; j++)
		{
			for (size_t k = 0; k < 4; k++)
			{
				this->uStrings[i][j][k] = L"No text";
			}
		}*/

		//arcane
		this->uStrings[i][0][0] = L"X: Shorter projectile cooldown";
		this->uStrings[i][0][1] = L"Y: Higher projectile speed";
		this->uStrings[i][0][2] = L"A: Improves seeking";
		this->uStrings[i][0][3] = L"B: Projectile lives longer";

		//fire
		this->uStrings[i][1][0] = L"X: Stonger projectile";
		this->uStrings[i][1][1] = L"Y: Stronger stomp";
		this->uStrings[i][1][2] = L"A: Larger explotion Radius";
		this->uStrings[i][1][3] = L"B: Increase your maximum health";

		//wind
		this->uStrings[i][2][0] = L"X: Faster projectile";
		this->uStrings[i][2][1] = L"Y: Stronger stomp";
		this->uStrings[i][2][2] = L"A: Stronger projectiles";
		this->uStrings[i][2][3] = L"B: Shorter dash cooldown";

		//earth
		this->uStrings[i][3][0] = L"X: Faster projectile";
		this->uStrings[i][3][1] = L"Y: Larger projectile effect radius";
		this->uStrings[i][3][2] = L"A: You take less pushback";
		this->uStrings[i][3][3] = L"B: Stronger stomp";

		//water
		this->uStrings[i][4][0] = L"X: Faster projectile";
		this->uStrings[i][4][1] = L"Y: Larger stomp distance";
		this->uStrings[i][4][2] = L"A: Stronger projectles";
		this->uStrings[i][4][3] = L"B: Shorter wall cooldown";
	}

	float offsetX = WIDTH / 2.0f;
	float offsetY = HEIGHT / 2.0f;

	this->textPos[0][0] = { offsetX / 2 - 42, 73};
	this->textPos[1][0] = { offsetX / 2 - 42 + offsetX, 73.0f };
	this->textPos[2][0] = { offsetX / 2 - 42, 73.0f + offsetY };
	this->textPos[3][0] = { offsetX / 2 - 42 + offsetX, 73.0f + offsetY };

	this->textPos[0][1] = { 180.0f / 2 + 24.0f,  293.0f / 2 };
	this->textPos[0][2] = { 180.0f / 2 + 24.0f, 389.0f / 2 };
	this->textPos[0][3] = { 180.0f / 2 + 24.0f, 480.0f / 2 };
	this->textPos[0][4] = { 180.0f / 2 + 24.0f, 579.0f / 2 };

	
	this->textPos[1][1] = { 180.0f / 2 + 24.0f + offsetX , 293.0f / 2 };
	this->textPos[1][2] = { 180.0f / 2 + 24.0f + offsetX , 389.0f / 2 };
	this->textPos[1][3] = { 180.0f / 2 + 24.0f + offsetX , 480.0f / 2 };
	this->textPos[1][4] = { 180.0f / 2 + 24.0f + offsetX ,  579.0f / 2 };

	this->textPos[2][1] = { 180.0f / 2 + 24.0f,  293.0f / 2 + offsetY };
	this->textPos[2][2] = { 180.0f / 2 + 24.0f, 389.0f / 2 + offsetY };
	this->textPos[2][3] = { 180.0f / 2 + 24.0f, 480.0f / 2 + offsetY };
	this->textPos[2][4] = { 180.0f / 2 + 24.0f, 579.0f / 2 + offsetY };

	this->textPos[3][1] = { 180.0f / 2 + 24.0f + offsetX , 293.0f / 2 + offsetY };
	this->textPos[3][2] = { 180.0f / 2 + 24.0f + offsetX , 389.0f / 2 + offsetY };
	this->textPos[3][3] = { 180.0f / 2 + 24.0f + offsetX , 480.0f / 2 + offsetY };
	this->textPos[3][4] = { 180.0f / 2 + 24.0f + offsetX , 579.0f / 2 + offsetY };

	this->eTextPos[0][0] = { WIDTH * 0.149f, HEIGHT * 0.08f};
	this->eTextPos[0][1] = { WIDTH * 0.149f, HEIGHT * 0.15f };
	this->eTextPos[0][2] = { WIDTH * 0.149f, HEIGHT * 0.22f };
	this->eTextPos[0][3] = { WIDTH * 0.149f, HEIGHT * 0.29f };
	this->eTextPos[0][4] = { WIDTH * 0.149f, HEIGHT * 0.36f };

	this->eTextPos[1][0] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.08f };
	this->eTextPos[1][1] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.15f };
	this->eTextPos[1][2] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.22f };
	this->eTextPos[1][3] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.29f };
	this->eTextPos[1][4] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.36f };

	this->eTextPos[2][0] = { WIDTH * 0.149f, HEIGHT * 0.08f + offsetY };
	this->eTextPos[2][1] = { WIDTH * 0.149f, HEIGHT * 0.15f + offsetY };
	this->eTextPos[2][2] = { WIDTH * 0.149f, HEIGHT * 0.22f + offsetY };
	this->eTextPos[2][3] = { WIDTH * 0.149f, HEIGHT * 0.29f + offsetY };
	this->eTextPos[2][4] = { WIDTH * 0.149f, HEIGHT * 0.36f + offsetY };

	this->eTextPos[3][0] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.08f + offsetY };
	this->eTextPos[3][1] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.15f + offsetY };
	this->eTextPos[3][2] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.22f + offsetY };
	this->eTextPos[3][3] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.29f + offsetY };
	this->eTextPos[3][4] = { WIDTH * 0.149f + offsetX, HEIGHT * 0.36f + offsetY };
	

	this->eStrings[0] = L"X: Arcane elemetal powers";
	this->eStrings[1] = L"Y: Fire elemetal powers";
	this->eStrings[2] = L"A: Wind elemetal powers";
	this->eStrings[3] = L"B: Earth elemetal powers";
	this->eStrings[4] = L"RB: Water elemetal powers";

	this->eTextColor[0] = XMVectorSet(.588f, 1.f, .965f, 1.f);
	this->eTextColor[1] = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	this->eTextColor[2] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	this->eTextColor[3] = XMVectorSet(0.f, .580f, 1.f, 1.f);
	this->eTextColor[4] = XMVectorSet(1.f, .929f, .400f, 1.f);
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
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->cuMenuTexture);
			for (size_t i = 1; i < 5; i++)
			{
				renderer->gDeviceContext->PSSetShaderResources(i, 1, &renderer->cuMenuTexture);
			}

			for (size_t i = 1; i < map->nrOfPlayers +1; i++)
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

			for (size_t i = 0; i < map->nrOfPlayers; i++)
			{
				renderer->gDeviceContext->PSSetShaderResources((i + 1), 1, &renderer->r1CUTextures[map->playerElemnts[i]]);
			}


			renderer->gDeviceContext->Draw(24, 0);
			this->setUpgradesArrowPos(map);
			this->drawUpgradeText(map);

		}
		else if (currentState == GameState::EndGame)
		{
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->endMenuTexture);
			renderer->gDeviceContext->Draw(6, 0);
		}
		

	}
	if (currentState != GameState::MainMenu && currentState != GameState::EndGame && currentState != GameState::ChoosePowers)
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

		m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Round: ") + std::to_wstring(gMapConstants.round)).c_str(), XMFLOAT2(WIDTH * 0.46875f, HEIGHT * 0.03125f), Colors::Black);
		if (map->nrOfPlayers == 2)
		{
			m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player 1: ") + std::to_wstring(map->playerPoints[0])).c_str(), XMFLOAT2(WIDTH * 0.390625f , HEIGHT* 0.0025f), Colors::Black);
			m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player 2: ") + std::to_wstring(map->playerPoints[1])).c_str(), XMFLOAT2(WIDTH * 0.52734375f, HEIGHT* 0.0625f), Colors::Black);
		}
		else
		{
			m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player 1: ") + std::to_wstring(map->playerPoints[0])).c_str(), XMFLOAT2(WIDTH * 0.390625f, HEIGHT* 0.0025f), Colors::Black);
			m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player 2: ") + std::to_wstring(map->playerPoints[1])).c_str(), XMFLOAT2(WIDTH * 0.52734375f, HEIGHT* 0.0025f), Colors::Black);
		}
		
		if (map->nrOfPlayers >= 3)
		{
			if (map->nrOfPlayers == 3)
			{
				m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player 3: ") + std::to_wstring(map->playerPoints[2])).c_str(), XMFLOAT2(WIDTH * 0.4625f, HEIGHT* 0.0625f), Colors::Black);
			}
			else
			{
				m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player 3: ") + std::to_wstring(map->playerPoints[2])).c_str(), XMFLOAT2(WIDTH * 0.390625f, HEIGHT* 0.06f), Colors::Black);
			}
			
		}
		if (map->nrOfPlayers == 4)
		{
			m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player 4: ") + std::to_wstring(map->playerPoints[3])).c_str(), XMFLOAT2(WIDTH * 0.52734375f, HEIGHT* 0.06f), Colors::Black);
		}
		
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
		for (size_t i = 0; i < map->nrOfPlayers; i++)
		{
			m_spriteBatch->Draw(m_Balltexture.Get(), this->arrowPos[i], nullptr, Colors::White, 0.f, m_origin);
		}
		
		this->setReady(map);
	}
	else if (currentState == GameState::ChoosePowers)
	{
		for (size_t i = 0; i < map->nrOfPlayers; i++)
		{
			m_spriteBatch->Draw(m_Balltexture.Get(), this->arrowPos[i], nullptr, Colors::White, 0.f, m_origin);
		}
		for (size_t i = 0; i < map->nrOfPlayers; i++)
		{
			for (size_t j = 0; j < 5; j++)
			{
				m_spriteFont->DrawString(m_spriteBatch.get(), this->eStrings[j].c_str(), this->eTextPos[i][j], this->eTextColor[j]);
			}
		}
		this->setReady(map);
	}
	auto pos = ImGui::GetIO();// .MousePos();
	//m_spriteFont->DrawString(m_spriteBatch.get(), L"No mouse required", XMFLOAT2(pos.MousePos.x-100.f, pos.MousePos.y), Colors::HotPink);

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
	float offsetX = WIDTH / 2;
	float offsetY = HEIGHT / 2;
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
		this->arrowPos[1].y = 293.0f / 2 ;
		this->arrowPos[1].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[1] == 1)
	{
		this->arrowPos[1].y = 389.0f / 2;
		this->arrowPos[1].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[1] == 2)
	{
		this->arrowPos[1].y = 480.0f / 2;
		this->arrowPos[1].x = 180.0f / 2 + offsetX;
	}
	if (map->upgradeChoice[1] == 3)
	{
		this->arrowPos[1].y = 579.0f / 2;
		this->arrowPos[1].x = 180.0f / 2 + offsetX;
	}

	//p3
	if (map->upgradeChoice[2] == 0)
	{
		this->arrowPos[2].y = 293.0f / 2 + offsetY;
		this->arrowPos[2].x = 180.0f / 2;
	}
	if (map->upgradeChoice[2] == 1)
	{
		this->arrowPos[2].y = 389.0f / 2 + offsetY;
		this->arrowPos[2].x = 180.0f / 2 ;
	}
	if (map->upgradeChoice[2] == 2)
	{
		this->arrowPos[2].y = 480.0f / 2 + offsetY;
		this->arrowPos[2].x = 180.0f / 2;
	}
	if (map->upgradeChoice[2] == 3)
	{
		this->arrowPos[2].y = 579.0f / 2 + offsetY;
		this->arrowPos[2].x = 180.0f / 2 ;
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
	float offsetX = WIDTH / 2;
	float offsetY = HEIGHT / 2;

	//player 1
	if (map->playerElemnts[0] == 0)
	{
		this->arrowPos[0].x = this->eTextPos[0][0].x - 29.5f;
		this->arrowPos[0].y = this->eTextPos[0][0].y;
	}
	if (map->playerElemnts[0] == 1)
	{
		this->arrowPos[0].x = this->eTextPos[0][1].x - 29.5f;
		this->arrowPos[0].y = this->eTextPos[0][1].y;
	}
	if (map->playerElemnts[0] == 2)
	{
		this->arrowPos[0].x = this->eTextPos[0][2].x - 29.5f;
		this->arrowPos[0].y = this->eTextPos[0][2].y;
	}
	if (map->playerElemnts[0] == 3)
	{
		this->arrowPos[0].x = this->eTextPos[0][3].x - 29.5f;
		this->arrowPos[0].y = this->eTextPos[0][3].y;
	}
	if (map->playerElemnts[0] == 4)
	{
		this->arrowPos[0].x = this->eTextPos[0][4].x - 29.5f;
		this->arrowPos[0].y = this->eTextPos[0][4].y;
	}

	//player 2
	if (map->playerElemnts[1] == 0)
	{
		this->arrowPos[1].x = this->eTextPos[1][0].x - 29.5f;
		this->arrowPos[1].y = this->eTextPos[1][0].y;
	}
	if (map->playerElemnts[1] == 1)
	{
		this->arrowPos[1].x = this->eTextPos[1][1].x - 29.5f;
		this->arrowPos[1].y = this->eTextPos[1][1].y;
	}
	if (map->playerElemnts[1] == 2)
	{
		this->arrowPos[1].x = this->eTextPos[1][2].x - 29.5f;
		this->arrowPos[1].y = this->eTextPos[1][2].y;
	}
	if (map->playerElemnts[1] == 3)
	{
		this->arrowPos[1].x = this->eTextPos[1][3].x - 29.5f;
		this->arrowPos[1].y = this->eTextPos[1][3].y;
	}
	if (map->playerElemnts[1] == 4)
	{
		this->arrowPos[1].x = this->eTextPos[1][4].x - 29.5f;
		this->arrowPos[1].y = this->eTextPos[1][4].y;
	}

	//player 3
	if (map->playerElemnts[2] == 0)
	{
		this->arrowPos[2].x = this->eTextPos[2][0].x - 29.5f;
		this->arrowPos[2].y = this->eTextPos[2][0].y;
	}
	if (map->playerElemnts[2] == 1)
	{
		this->arrowPos[2].x = this->eTextPos[2][1].x - 29.5f;
		this->arrowPos[2].y = this->eTextPos[2][1].y;
	}
	if (map->playerElemnts[2] == 2)
	{
		this->arrowPos[2].x = this->eTextPos[2][2].x - 29.5f;
		this->arrowPos[2].y = this->eTextPos[2][2].y;
	}
	if (map->playerElemnts[2] == 3)
	{
		this->arrowPos[2].x = this->eTextPos[2][3].x - 29.5f;
		this->arrowPos[2].y = this->eTextPos[2][3].y;
	}
	if (map->playerElemnts[2] == 4)
	{
		this->arrowPos[2].x = this->eTextPos[2][4].x - 29.5f;
		this->arrowPos[2].y = this->eTextPos[2][4].y;
	}

	//player 4
	if (map->playerElemnts[3] == 0)
	{
		this->arrowPos[3].x = this->eTextPos[3][0].x - 29.5f;
		this->arrowPos[3].y = this->eTextPos[3][0].y;
	}
	if (map->playerElemnts[3] == 1)
	{
		this->arrowPos[3].x = this->eTextPos[3][1].x - 29.5f;
		this->arrowPos[3].y = this->eTextPos[3][1].y;
	}
	if (map->playerElemnts[3] == 2)
	{
		this->arrowPos[3].x = this->eTextPos[3][2].x - 29.5f;
		this->arrowPos[3].y = this->eTextPos[3][2].y;
	}
	if (map->playerElemnts[3] == 3)
	{
		this->arrowPos[3].x = this->eTextPos[3][3].x - 29.5f;
		this->arrowPos[3].y = this->eTextPos[3][3].y;
	}
	if (map->playerElemnts[3] == 4)
	{
		this->arrowPos[3].x = this->eTextPos[3][4].x - 29.5f;
		this->arrowPos[3].y = this->eTextPos[3][4].y;
	}

}

void Menu::setReady(Map * map)
{
	float offsetX = WIDTH / 2;
	float offsetY = HEIGHT / 2;
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
	if (this->ready[2] && map->nrOfPlayers == 3)
	{
		m_spriteBatch->Draw(m_readyTexture.Get(), DirectX::XMFLOAT2(0.0f, 0.0f + offsetY), nullptr, Colors::White, 0.f, m_origin);
	}
	if (this->ready[3] && map->nrOfPlayers == 4)
	{
		m_spriteBatch->Draw(m_readyTexture.Get(), DirectX::XMFLOAT2(0.0f + offsetX, 0.0f + offsetY), nullptr, Colors::White, 0.f, m_origin);
	}


}

void Menu::drawUpgradeText(Map * map)
{

	for (size_t i = 0; i < map->nrOfPlayers; i++)
	{
		//round syns redan på upgrade menyn
		//m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Round: ") + std::to_wstring(gMapConstants.round)).c_str(), this->textPos[i][0], Colors::Black);
		for (size_t j = 1; j < 5; j++)
		{
			XMVECTOR col;
			if (map->playerElemnts[i] == 0)//Arcane
			{
				col = XMVectorSet(.588f, 1.f, .965f, 1.f);
			}
			else if (map->playerElemnts[i] == 1)//Fire
			{
				col = XMVectorSet(1.f, 1.f, 1.f, 1.f);
			}
			else if (map->playerElemnts[i] == 2)//Wind
			{
				col = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			}
			else if (map->playerElemnts[i] == 3)//Earth
			{
				col = XMVectorSet(0.f, .580f, 1.f, 1.f);
			}
			else if (map->playerElemnts[i] == 4)//Water
			{
				col = XMVectorSet(1.f, .929f, .400f, 1.f);
			}
			m_spriteFont->DrawString(m_spriteBatch.get(), this->uStrings[gMapConstants.round - 1][map->playerElemnts[i]][j - 1].c_str(), this->textPos[i][j], col);
		}
	}
}

void Menu::setSelectedPos(GameState currentState)
{
	if (currentState == GameState::MainMenu)
	{
		if (selectedButton == 0)
		{
			catPos.x = WIDTH * 0.29296875f;
			catPos.y = HEIGHT * 0.28125f;
		}
		else if (selectedButton == 1) {
			catPos.x = WIDTH * 0.29296875f;
			catPos.y = HEIGHT * 0.4375f;
		}
		else if (selectedButton == 2) {
			catPos.x = WIDTH * 0.29296875f;
			catPos.y = HEIGHT * 0.6f;
		}
		else if (selectedButton == 3) {
			catPos.x = WIDTH * 0.29296875f;
			catPos.y = HEIGHT * 0.7625f;
		}
	}
}
