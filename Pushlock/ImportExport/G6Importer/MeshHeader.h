#pragma once
#include <stdint.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
using std::string;
using std::vector;

struct SkinnedVertex
{
	float posX, posY, posZ;
	float norX, norY, norZ;
	float u, v;
	int boneIndex[4];
	float boneWeights[4];
};

struct Vertex
{
	float posX, posY, posZ;
	float norX, norY, norZ;
	float u, v;
};

struct UVSet
{
	int id;
	int name_length;
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

struct JointPose
{
	DirectX::XMVECTOR   m_rot;      //Q
	DirectX::XMVECTOR   m_trans;    //T    
	float				m_scale;    //S     
};

struct AnimationSample
{
	vector<JointPose> m_aJointPose; //array of joint poses 
};

struct AnimationClip
{
	vector<Joint> m_pSkeleton;
	float m_framesPerSecond;
	uint32_t m_frameCount;
	vector<AnimationSample> m_aSamples; //array of samples
	bool m_isLooping;
};

struct Skeleton
{
	uint32_t m_jointCount;          //number of joints
	vector<Joint> m_aJoint;         //array of joints (hierarchy)
};



struct MeshHeader
{
	uint8_t meshNameLength;
	//name

	uint32_t numberOfVerts;
	//verts

	uint32_t numberOfIndex;
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

	uint32_t numberOfJoints;
	//joints


	//ANIMATIONCLIP
	uint32_t framesPerSecond;
	uint32_t frameCount;

	//Not found in FBX, needs to be set manually somehow.
	bool isLooping;
};

struct sMesh
{
	MeshHeader header;
	string name;
	vector<Vertex> verts;

	vector<int> indices;
	bool is_skinned;
	uint8_t mat_id;
	//vector<UV> uvs;
};


struct sSkinnedMesh
{
	SkinnedMeshHeader header;
	string name;
	vector<SkinnedVertex> verts;
	vector<Joint> skeletonHierarchy;
	//vector<UVSet> uvsets;
	vector<int> indices;
	bool is_skinned;		//unused
	uint8_t mat_id;
	//vector<UV> uvs;
	AnimationClip animation; //default animation, presumably idle_anim
};











