#include "Animator.h"



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
			currentJoint.globalTransform = skeleton->m_aJoint[currentJoint.parent_id].globalTransform * currentJoint.localTransform;

		//Calculate final matrix (skinning matrix)
		aFinalMatrices[i] = DirectX::XMMatrixMultiply(currentJoint.globalTransform, currentJoint.inverseBindPose);
	}

	//All matrices have been updated. We could upload them to the GPU here. (i think..)

}

void Animator::Update(float deltaTime)
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
