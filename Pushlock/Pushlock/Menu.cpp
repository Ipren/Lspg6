#include "Menu.h"
#include "Globals.h"
#include "DirectXTK.h"
#include <DirectXMath.h>

Menu::Menu(Renderer* renderer)
{

	/*ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(renderer->gDevice, L"test_main_menu.dds", &r, &m_texture, 0, nullptr));*/

	m_spriteBatch = std::make_unique<SpriteBatch>(renderer->gDeviceContext);
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(renderer->gDeviceContext);

	this->selectedButton = 0;

	//main menu buttons
	std::vector<bool>* butts = new std::vector<bool>();
	butts->push_back(false);//start game with 2 players	
	butts->push_back(false);//start game with 3 players
	butts->push_back(false);//start game with 4 players
	this->buttons.push_back(*butts);

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

void Menu::render(Renderer* renderer, GameState currentState)
{
	/*m_spriteBatch->Begin();

	m_spriteBatch->Draw(m_texture.Get(), m_screenPos, nullptr, Colors::White, 0.f, m_origin);

	m_spriteBatch->End();*/

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
			renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->mainMenuTexture);
		}
		renderer->gDeviceContext->Draw(6, 0);

	}
	

	//renderer->gDeviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	//renderer->gDeviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	//renderer->gDeviceContext->RSSetState(m_states->CullNone());

	//m_effect->Apply(renderer->gDeviceContext);

	//renderer->gDeviceContext->IASetInputLayout(m_inputLayout.Get());

	
}

void Menu::selectDown(GameState currentState)
{
	if (buttons[currentState].size()-1 > selectedButton)
	{
		selectedButton++;
	}
	else
	{
		selectedButton = 0;
	}
}

void Menu::selectUp(GameState currentState)
{
	if (0 < selectedButton)
	{
		selectedButton--;
	}
	else
	{
		selectedButton = buttons[currentState].size()-1;
	}
}
