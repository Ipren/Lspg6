#include "Menu.h"
#include "Globals.h"
#include "DirectXTK.h"
#include <DirectXMath.h>

Menu::Menu(Renderer* renderer)
{

	ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(renderer->gDevice, L"cat.dds", &r, &m_texture, 0, nullptr));
	DXCALL(CreateDDSTextureFromFile(renderer->gDevice, L"../Resources/textures/player1.dds", &r, &m_winner, 0, nullptr));
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
	m_spriteBatch.reset();

	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
}

#include "imgui.h"

void Menu::render(Renderer* renderer, GameState currentState, int winner)
{


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
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->cpMenuTexture);
		}
		else if (currentState == GameState::MainMenu)
		{
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->mainMenuTexture);
		}
		else if (currentState == GameState::EndRound)
		{
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->cuMenuTexture);
		}
		else if (currentState == GameState::EndGame)
		{
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->endMenuTexture);
		}
		renderer->gDeviceContext->Draw(6, 0);

	}

	m_spriteBatch->Begin();
	if (currentState == GameState::MainMenu)
		m_spriteBatch->Draw(m_texture.Get(), catPos, nullptr, Colors::White, 0.f, m_origin);
	else if (currentState == GameState::EndGame)
	{
		/*std::wstring s = L"Player: " + (winner + 1);
		wchar_t* c = new wchar_t(&s.c_str());*/
		m_spriteFont->DrawString(m_spriteBatch.get(), (std::wstring(L"Player: ") + std::to_wstring(winner+1)).c_str(), XMFLOAT2(375, 300), Colors::HotPink);
	}
	auto pos = ImGui::GetIO();// .MousePos();
	m_spriteFont->DrawString(m_spriteBatch.get(), L"Detta ar inte en fin font", XMFLOAT2(pos.MousePos.x, pos.MousePos.y), Colors::HotPink);

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
