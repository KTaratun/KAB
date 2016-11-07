#pragma once
#include <vector>

// Store needed mesh data here.
struct Vertex
{
	float xyz[3];
	float uvw[3];
	float normals[3];
	float bin[3];
	float tan[3];
	DirectX::XMINT4 bone;
	DirectX::XMFLOAT4 weights;
};

struct BlendingIndexWeightPair
{
	unsigned int blendingIndex;
	double blendingWeight;

	BlendingIndexWeightPair() :
		blendingIndex(0),
		blendingWeight(0)
	{}
};

struct CtrlPoint
{
	DirectX::XMFLOAT3 pos;
	std::vector<BlendingIndexWeightPair> blendingInfo;

	CtrlPoint()
	{
		blendingInfo.reserve(4);
	}
};

class Mesh
{
	
public:
	Mesh();
	~Mesh();
	std::vector<Vertex> verts;
};