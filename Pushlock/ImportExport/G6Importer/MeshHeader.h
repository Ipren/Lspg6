#pragma once
#include <stdint.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
using std::string;
using std::vector;

struct MeshHeader
{
	uint8_t meshNameLength;
	//name

	uint32_t numberOfVerts;
	//verts

	uint32_t numberOfIndex;
	//index
	
	uint32_t numberOfUVSets;
	//index
};

struct SkinnedMeshHeader
{
	uint8_t meshNameLength;
	//name

	uint32_t numberOfVerts;
	//Skinned verts

	uint32_t numberOfIndex;
	//index

	uint32_t numberOfUVSets;
	//uvsets

	uint32_t numberOfJoints;
	//joints


	//ANIMATIONCLIP
	uint32_t framesPerSecond;
	uint32_t frameCount;
	bool isLooping;
};

struct UVSet
{
	int id;
	int name_length;
	string name;
};

struct Vertex
{
	float posX, posY, posZ;
	float norX, norY, norZ;
	float u, v;
	//uv1
	//uv2
	//uv3
	
	//other stuff?
};

struct SkinnedVertex
{
	float posX, posY, posZ;
	float norX, norY, norZ;

	int boneIndex[4];
	float boneWeights[4];
	//uv1
	//uv2
	//uv3

	//other stuff?
};

struct UV {
	float U, V;
};

struct Joint {
	DirectX::XMMATRIX inverseBindPose;
	DirectX::XMMATRIX localTransform;
	DirectX::XMMATRIX globalTransform;

	int parent_id;
	string name;
};

struct sMesh
{
	MeshHeader header;
	string name;
	vector<Vertex> verts;
	vector<UVSet> uvsets;
	vector<int> indices;
	bool is_skinned;
	uint8_t mat_id;
	vector<UV> uvs;
};



struct JointPose
{
	DirectX::XMVECTOR   m_rot;      //Q
	DirectX::XMVECTOR   m_trans;    //T    
	float				m_scale;    //S     
};

//struct SkeletonPose
//{
//	vector<Joint>			m_pSkeleton;    //skeleton + num joints
//	vector<JointPose>		m_aLocalPose;   //local joint poses
//	DirectX::XMFLOAT4X4A*	m_aGlobalPose;  //global joint poses
//};

struct AnimationSample
{
	vector<JointPose> m_aJointPose; //array of joint poses 
};

struct Skeleton
{
	uint32_t m_jointCount;          //number of joints
	vector<Joint> m_aJoint;         //array of joints (hierarchy)
};

struct AnimationClip
{
	vector<Joint> m_pSkeleton;
	float m_framesPerSecond;
	uint32_t m_frameCount;
	vector<AnimationSample> m_aSamples; //array of samples
	bool m_isLooping;

	//AnimationClip() {};
	//AnimationClip(const AnimationClip& other)
	//{
	//	this->m_pSkeleton = other.m_pSkeleton;
	//	this->m_frameCount = other.m_frameCount;
	//	this->m_framesPerSecond = other.m_framesPerSecond;
	//	this->m_isLooping = other.m_isLooping;
	//	this->m_aSamples = new AnimationSample[this->m_framesPerSecond];

	//	for (int i = 0; i < this->m_frameCount; i++)
	//	{
	//		m_aSamples[i] = other.m_aSamples[i];
	//	}
	//}
	//~AnimationClip()
	//{
	//	delete[] m_aSamples;
	//}
	//AnimationClip& operator= (const AnimationClip& other)
	//{
	//	m_pSkeleton = other.m_pSkeleton;
	//	m_frameCount = other.m_frameCount;
	//	m_framesPerSecond = other.m_framesPerSecond;
	//	m_isLooping = other.m_isLooping;
	//	m_aSamples = new AnimationSample[this->m_framesPerSecond];

	//	for (int i = 0; i < m_frameCount; i++)
	//	{
	//		m_aSamples[i] = other.m_aSamples[i];
	//	}
	//	return *this;
	//}
};

struct sSkinnedMesh
{
	SkinnedMeshHeader header;
	string name;
	vector<SkinnedVertex> verts;
	vector<Joint> skeletonHierarchy;
	vector<UVSet> uvsets;
	vector<int> indices;
	bool is_skinned;
	uint8_t mat_id;
	vector<UV> uvs;
	AnimationClip animation;
};