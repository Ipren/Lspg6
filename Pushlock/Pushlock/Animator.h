#pragma once
#include "MeshHeader.h"
#include "G6Import.h"

enum AnimationState
{
	PLAYING,
	PAUSED
};

class Animator
{
public:
	Animator();
	~Animator();

	//MEMBERS
	ID3D11Buffer* cbJointTransforms = nullptr;
	ID3D11Buffer* cbTestMatrix = nullptr;

	AnimationState state = PLAYING;
	
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

	ID3D11VertexShader *skinned_vs;
	ID3D11PixelShader *skinned_ps;
	ID3D11InputLayout *skinned_layout;

	ID3D11Buffer* pVertexBuffer;

	Skeleton* skeleton = nullptr;     //skeleton to animate

	vector<DirectX::XMMATRIX> aFinalMatrices;	//AKA skinningMatrix, the final calculated matrices, one for each joint in the skeleton.
										//We could have each joint in the skeleton have a Matrix finalMatrix; and just update
										//each joints matrix.
										//But lets try this for now.

	sSkinnedMesh* mesh = nullptr;      //mesh to animate. Needs to use appropriate skeleton. 
							//Could be checked during intialization.
							

	//SkeletonPose skeletonPose;    //This structure holds the CURRENT localPoses (QTS)
									//aswell as the GlobalPoses.
									//We could have the Animator class own these instead.

	AnimationClip* currentClip = nullptr;     //Currently playing clip.

	float currentTimeInClip = 0.0f;         //Keeps track of where in the clips "timeline" we are, in seconds.
											//currentClip keeps track of framesPerSecond of the clip.

	//FUNCTIONS

											//**NOTE**
											//We are assuming matrix multiplication order is in the order of OpenGL, which if
											//i recall correctly is opposite in DirectX. 
											//**NOTE**

	void CalculateInitialGlobalTransforms();

	//void CalculateInverseBindPose(); <--,
	//We don't need this method since we calculate this during import from FBX.
	//We could do it here instead to save filespace.
	//The inverseBindPose of a joint is simply the inverse of the joints initial Global Transform.

	//**TODO**
	//Interpolates between two keyFrames.
	//**TODO**
	void InterpolateKeyframes(const AnimationSample& last, const AnimationSample& next, float progression);

	//Calculate final matrix. This is done every frame, after the local transforms of the joints
	//have been updated/animated.
	void CalculateFinalMatrices();


	void CreateBuffersAndShaders();

	//Called from the engine. 
	void Update(float deltaTime);
	void DrawAndUpdate(float deltaTime);
	void PreDraw();

	void LoadSkinnedMesh(const char* filename);

	void UpdateConstantBuffers();

	//Setters
	void SetState(AnimationState newState) { this->state = newState; }
	void SetAnimationClip(string clipName);
	void SetAnimationClip(AnimationClip* clip);

};

