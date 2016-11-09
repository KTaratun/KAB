#pragma once
#include <vector>
#include <fbxsdk.h>

// Store needed mesh data here.
struct Vertex
{
	DirectX::XMFLOAT3 xyz;
	DirectX::XMFLOAT3 uvw;
	DirectX::XMFLOAT3 normals;
	DirectX::XMFLOAT3 bin;
	DirectX::XMFLOAT3 tan;
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

struct BinaryHeader
{
	int file_size;
	int vector_size;
	int CtrlPointIndicies_size;
	int fileversion;
	FbxLocalTime timestamp;
};