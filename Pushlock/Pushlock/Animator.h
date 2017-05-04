#pragma once
#include "MeshHeader.h"
class Animator
{
public:
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

	void CalculateInitialGlobalTransforms()
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

	//void CalculateInverseBindPose(); <--,
	//We don't need this method since we calculate this during import from FBX.
	//We could do it here instead to save filespace.
	//The inverseBindPose of a joint is simply the inverse of the joints initial Global Transform.

	//**TODO**
	//Interpolates between two keyFrames.
	//**TODO**
	void InterpolateKeyframes(const AnimationSample& last, const AnimationSample& next, float progression)
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

	//Calculate final matrix. This is done every frame, after the local transforms of the joints
	//have been updated/animated.
	void CalculateFinalMatrices()
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
				currentJoint.globalTransform = skeleton->m_aJoint[currentJoint.parent_id].globalTransform * currentJoint.localTransform;

			//Calculate final matrix (skinning matrix)
			aFinalMatrices[i] = DirectX::XMMatrixMultiply(currentJoint.globalTransform, currentJoint.inverseBindPose);
		}

		//All matrices have been updated. We could upload them to the GPU here. (i think..)

	}



	//Called from the engine. 
	void Update(float deltaTime)
	{
		//Update currentTimeInClip
		currentTimeInClip += deltaTime;
		if (currentTimeInClip > currentClip->m_frameCount / currentClip->m_framesPerSecond)
			currentTimeInClip -= currentClip->m_frameCount / currentClip->m_framesPerSecond;

		//Previous keyframe (next keyframe is prevIndex+1)
		int prevIndex = std::floor(currentClip->m_framesPerSecond*currentTimeInClip);
		float progression = std::fmod(currentClip->m_framesPerSecond, currentTimeInClip);

		//find previous and next keyframes (AnimationSamples contained in currentClip), and interpolate between them
		AnimationSample& prev = currentClip->m_aSamples[prevIndex];
		AnimationSample& next = currentClip->m_aSamples[prevIndex + 1];

		InterpolateKeyframes(prev, next, progression);

		//joints' local transforms have been updated; calculate final matrices.
		CalculateFinalMatrices();
	}
};

