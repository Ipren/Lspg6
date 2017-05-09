#include "Animator.h"
#include "Helpers.h"
#include "dxerr.h"
#include "Globals.h"
Animator::Animator()
{
}


Animator::~Animator()
{
}

void Animator::CalculateInitialGlobalTransforms()
{
	for (int i = 0; i < skeleton->m_jointCount; i++)
	{
		Joint& j = skeleton->m_aJoint[i];
		if (j.parent_id == -1)
			j.globalTransform = j.localTransform;
		else
			j.globalTransform = skeleton->m_aJoint[j.parent_id].globalTransform * j.localTransform;
	}
}

void Animator::InterpolateKeyframes(const AnimationSample& last, const AnimationSample& next, float progression)
{
	int i = 0;
	for (auto& joint : skeleton->m_aJoint)
	{
		//Interpolate rotation
		DirectX::XMVECTOR rot = DirectX::XMQuaternionSlerp(last.m_aJointPose[i].m_rot, next.m_aJointPose[i].m_rot, progression);
		//Interpolate translation
		DirectX::XMVECTOR trans = DirectX::XMVectorLerp(last.m_aJointPose[i].m_trans, next.m_aJointPose[i].m_trans, progression);
		//Assign matrix to joint
		joint.localTransform = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f }, { 0,0,0 }, rot, trans);
		i++;
	}
}

void Animator::CalculateFinalMatrices()
{
	for (int i = 0; i < skeleton->m_jointCount; i++)
	{
		Joint& currentJoint = skeleton->m_aJoint[i];  // <--,
													  //struct Joint
													  //{
													  //    Float4x4 m_invereseBindPose;    //inverse bind pose transform
													  //    string m_name;                  //joint name
													  //    uint8_t m_iParent;              //parent index (-1 if this=root)
													  //};

													  //Calculate each joints new global Transform.
													  //The new global transform depends on the newly animated local transform.
		if (currentJoint.parent_id == -1)
			currentJoint.globalTransform = currentJoint.localTransform;
		else
			currentJoint.globalTransform = DirectX::XMMatrixMultiply(currentJoint.localTransform, skeleton->m_aJoint[currentJoint.parent_id].globalTransform);

		//Calculate final matrix (skinning matrix)
		aFinalMatrices[i] = DirectX::XMMatrixMultiply(currentJoint.inverseBindPose, currentJoint.globalTransform);
	}

	//All matrices have been updated. We could upload them to the GPU here. (i think..)

}

void Animator::CreateBuffersAndShaders()
{
	//Vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(SkinnedVertex) * this->mesh->verts.size();

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &mesh->verts[0];
	globalDevice->CreateBuffer(&bufferDesc, &data, &pVertexBuffer);




	ID3DBlob *blob = compile_shader(L"Skinned.hlsl", "VS", "vs_5_0", globalDevice);
	DXCALL(globalDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &skinned_vs));

	//int offset = 0;
	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
		//{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//custom_mesh_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, device);
	HRESULT hr = globalDevice->CreateInputLayout(input_desc, 5, blob->GetBufferPointer(), blob->GetBufferSize(), &this->skinned_layout);
	if (FAILED(hr))
	{
		MessageBox(0, L" input desc creation failed", L"error", MB_OK);
	}

	blob = compile_shader(L"Skinned.hlsl", "PS", "ps_5_0", globalDevice);
	DXCALL(globalDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &skinned_ps));
	blob->Release();

	//Constant buffer
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(XMMATRIX)*aFinalMatrices.size();
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &aFinalMatrices[0];
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	hr = this->device->CreateBuffer(&cbDesc, &InitData, &cbJointTransforms);

	if (FAILED(hr))
		MessageBox(0, L"cbJointTransforms buffer creation failed", L"error", MB_OK);

	///TESTMATRIX BUFFER
	//Constant buffer
	D3D11_BUFFER_DESC cbTest;
	cbTest.ByteWidth = sizeof(XMMATRIX);
	cbTest.Usage = D3D11_USAGE_DYNAMIC;
	cbTest.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbTest.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbTest.MiscFlags = 0;
	cbTest.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitTestData;
	InitTestData.pSysMem = &aFinalMatrices[0];
	InitTestData.SysMemPitch = 0;
	InitTestData.SysMemSlicePitch = 0;

	hr = this->device->CreateBuffer(&cbTest, &InitTestData, &cbTestMatrix);

	if (FAILED(hr))
		MessageBox(0, L"cbJointTransforms buffer creation failed", L"error", MB_OK);
	
}

void Animator::Update(float deltaTime)
{
	float frameTime = 1 / currentClip->m_framesPerSecond;

	//Update currentTimeInClip
	currentTimeInClip += deltaTime;

	if (currentTimeInClip > frameTime * currentClip->m_frameCount)
		currentTimeInClip -= frameTime * currentClip->m_frameCount;

	//if (currentTimeInClip > currentClip->m_frameCount / currentClip->m_framesPerSecond)
	//	currentTimeInClip -= currentClip->m_frameCount / currentClip->m_framesPerSecond;

	//Previous keyframe (next keyframe is prevIndex+1)
	int prevIndex = std::floorf(currentClip->m_framesPerSecond*currentTimeInClip);
	float progression = std::fmod(currentClip->m_framesPerSecond*currentTimeInClip, 1.0f);

	if (prevIndex >= currentClip->m_frameCount-1)
	{
		float neg = (1 / 24.0f) * (currentClip->m_frameCount-1);
		currentTimeInClip = currentTimeInClip - neg;

		prevIndex = std::floorf(currentClip->m_framesPerSecond*currentTimeInClip);
		progression = std::fmod(currentClip->m_framesPerSecond*currentTimeInClip, 1.0f);

	}
	//find previous and next keyframes (AnimationSamples contained in currentClip),
	//and interpolate between them
	if (prevIndex >= currentClip->m_aSamples.size()-1)
		MessageBox(0, L"prevIndex bigger than vector size", L"error", MB_OK);
	AnimationSample& prev = currentClip->m_aSamples[prevIndex];
	AnimationSample& next = currentClip->m_aSamples[prevIndex + 1];

	InterpolateKeyframes(prev, next, progression);

	//joints' local transforms have been updated; calculate final matrices.
	CalculateFinalMatrices();

}

//void Animator::AssignSkinnedMesh(string meshName)
//{
//	//this->mesh = meshcontainermap[meshName];
//	//this->skeleton = mesh->Get_sSkinnedMesh()->skeletonHierarchy; ~ish
//}

void Animator::LoadSkinnedMesh(const char * filename)
{
	this->device = globalDevice;
	this->deviceContext = globalDeviceContext;

	this->mesh = new sSkinnedMesh();
	G6Import::ImportAnimatedMesh(filename, this->mesh);
	this->currentClip = new AnimationClip(this->mesh->animation);
	this->skeleton = new Skeleton();
	skeleton->m_aJoint = this->mesh->skeletonHierarchy;
	skeleton->m_jointCount = skeleton->m_aJoint.size();
	skeleton->m_aJoint[0].parent_id = -1;
	this->aFinalMatrices.resize(skeleton->m_aJoint.size());
	CreateBuffersAndShaders();

	//TODO: Export fps and count
	this->currentClip->m_framesPerSecond = 24;
	this->currentClip->m_frameCount = 25;
}

void Animator::PreDraw()
{
	deviceContext->VSSetShader(skinned_vs, nullptr, 0);
	deviceContext->PSSetShader(skinned_ps, nullptr, 0);
	if (device == nullptr)
		return;

	UINT32 vertexSize = sizeof(SkinnedVertex);
	UINT32 offset = 0;
	deviceContext->VSSetConstantBuffers(10, 1, &this->cbTestMatrix);
	deviceContext->VSSetConstantBuffers(11, 1, &this->cbJointTransforms);
	deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &vertexSize, &offset);
	deviceContext->IASetInputLayout(this->skinned_layout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Animator::DrawAndUpdate(float deltaTime)
{
	//Do pre-draw stuff (set buffers and whatnot)
	PreDraw();

	//Update animation only if the state is set to PLAYING
	if (this->state = PLAYING)
	{
		this->Update(deltaTime);
		this->UpdateConstantBuffers();
	}

	//Draw.
	//TODO: DrawIndexed
	this->deviceContext->Draw(this->mesh->verts.size(), 0);
}

void Animator::UpdateConstantBuffers()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deviceContext->Map(cbJointTransforms, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, &aFinalMatrices[0], sizeof(XMMATRIX) * aFinalMatrices.size());

	deviceContext->Unmap(cbJointTransforms, 0);
}
