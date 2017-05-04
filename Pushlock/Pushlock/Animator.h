#pragma once
#include "MeshHeader.h"
class Animator
{
public:
	Animator();
	~Animator();

	//MEMBERS

	Skeleton* skeleton;     //skeleton to animate

	DirectX::XMMATRIX* aFinalMatrices;	//AKA skinningMatrix, the final calculated matrices, one for each joint in the skeleton.
										//We could have each joint in the skeleton have a Matrix finalMatrix; and just update
										//each joints matrix.
										//But lets try this for now.

	sSkinnedMesh* mesh;      //mesh to animate. Needs to use appropriate skeleton. 
							//Could be checked during intialization.
							//A skinned mesh keeps track of its buffers.

	SkeletonPose skeletonPose;      //This structure holds the CURRENT localPoses (QTS)
									//aswell as the GlobalPoses.
									//We could have the Animator class own these instead.

	AnimationClip* currentClip;     //Currently playing clip.

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



	//Called from the engine. 
	void Update(float deltaTime);
};

