#include "Menu.h"
#include "Globals.h"
#include "DirectXTK.h"
#include <DirectXMath.h>

Menu::Menu(Renderer* renderer)
{

	ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(renderer->gDevice, L"test_main_menu.dds", &r, &m_texture, 0, nullptr));

	m_spriteBatch = std::make_unique<SpriteBatch>(renderer->gDeviceContext);


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

	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(renderer->gDeviceContext);
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

void Menu::render(Renderer* renderer)
{
	/*m_spriteBatch->Begin();

	m_spriteBatch->Draw(m_texture.Get(), m_screenPos, nullptr, Colors::White, 0.f, m_origin);

	m_spriteBatch->End();*/

	//renderer->gDeviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	//renderer->gDeviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	//renderer->gDeviceContext->RSSetState(m_states->CullNone());

	//m_effect->Apply(renderer->gDeviceContext);

	//renderer->gDeviceContext->IASetInputLayout(m_inputLayout.Get());

	m_batch->Begin();

	for (int i = 0; i < this->quads.size(); i++)
	{
		DirectX::VertexPositionColor* vpc1;
		DirectX::VertexPositionColor* vpc2;
		DirectX::VertexPositionColor* vpc3;
		DirectX::VertexPositionColor* vpc4;
		for (int j = 0; j < 4; j++)
		{
			vpc1 = new DirectX::VertexPositionColor({ this->quads[i].pos[j].x, this->quads[i].pos[j].y, 0 }, { this->quads[i].color[i].x,this->quads[i].color[j].y,this->quads[i].color[j].z, });
			vpc2 = new DirectX::VertexPositionColor({ this->quads[i].pos[j].x, this->quads[i].pos[j].y, 0 }, { this->quads[i].color[i].x,this->quads[i].color[j].y,this->quads[i].color[j].z, });
			vpc3 = new DirectX::VertexPositionColor({ this->quads[i].pos[j].x, this->quads[i].pos[j].y, 0 }, { this->quads[i].color[i].x,this->quads[i].color[j].y,this->quads[i].color[j].z, });
			vpc4 = new DirectX::VertexPositionColor({ this->quads[i].pos[j].x, this->quads[i].pos[j].y, 0 }, { this->quads[i].color[i].x,this->quads[i].color[j].y,this->quads[i].color[j].z, });

		}


		m_batch->DrawQuad(*vpc1, *vpc2, *vpc3, *vpc4);
	}
	
	m_batch->End();
}

void Menu::addQuad(XMFLOAT2 pos1, XMFLOAT2 pos2, XMFLOAT2 pos3, XMFLOAT2 pos4, 
	XMFLOAT3 color1, XMFLOAT3 color2, XMFLOAT3 color3, XMFLOAT3 color4)
{
	Quad* q = new Quad();
	q->pos[0] = pos1;
	q->pos[1] = pos2;
	q->pos[2] = pos3;
	q->pos[3] = pos4;
	 
	q->color[0] = color1;
	q->color[1] = color2;
	q->color[2] = color3;
	q->color[3] = color4;

	this->quads.push_back(*q);
}
