#include "Menu.h"
#include "Globals.h"
#include "DirectXTK.h"
#include <DirectXMath.h>

Menu::Menu(Renderer* renderer)
{

	ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(renderer->gDevice, L"test_main_menu.dds", &r, &m_texture, 0, nullptr));

	m_spriteBatch = std::make_unique<SpriteBatch>(renderer->gDeviceContext);
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(renderer->gDeviceContext);


	m_screenPos.x = 100;
	m_screenPos.y = 100;
	m_origin.x = 0;
	m_origin.y = 0;


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
	if (currentState == GameState::ChoosePowers)
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
		renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->cpMenuTexture);

		renderer->gDeviceContext->Draw(6, 0);
	}
	else if(currentState == GameState::MainMenu)
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
		renderer->gDeviceContext->PSSetShaderResources(0, 1, &renderer->mainMenuTexture);

		renderer->gDeviceContext->Draw(6, 0);
	}
	

	//renderer->gDeviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	//renderer->gDeviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	//renderer->gDeviceContext->RSSetState(m_states->CullNone());

	//m_effect->Apply(renderer->gDeviceContext);

	//renderer->gDeviceContext->IASetInputLayout(m_inputLayout.Get());

	
}
