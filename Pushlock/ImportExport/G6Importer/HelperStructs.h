#pragma once
#include <Windows.h>
#include <vector>
#include <DirectXMath.h>


struct BlendingIndexWeightPair
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	BlendingIndexWeightPair() :
		mBlendingIndex(0),
		mBlendingWeight(0)
	{}
};

struct CtrlPoint
{
	DirectX::XMFLOAT3 mPosition;
	std::vector<BlendingIndexWeightPair> mBlendingInfo;

	CtrlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};
