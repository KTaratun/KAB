#pragma once
#include "FBXLoader.h"
#include "Interpolator.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#pragma comment(lib,"d3d11.lib")
#include <vector>
#include <stdio.h>
#include "modelClass.h"
#include "Blender.h"

using namespace DirectX;
using namespace std;
#define RELEASE_COM(x) {if (x){x->Release();x = nullptr;}}

class BoneSphere : public ModelClass
{
public:
	BoneSphere() {};
	~BoneSphere() {};

	void Initialize(ID3D11Device* device, XMMATRIX matrix);
	void Render(ID3D11DeviceContext* deviceContext, float delta);
	void Shutdown();
};

class MeshClass
{
	std::vector<Mesh> meshes;
	std::vector<TransformNode> transformHierarchy;
	std::vector<Animation> animations;
	std::string texture_name;
	std::vector< unsigned int > control_point_indices;
	std::vector<BoneSphere*> boneSpheres;
	std::vector<XMMATRIX> boneMatrices;
	BoneSphere* newboneSphere;
	KeyFrame* old;
	KeyFrame keyFrame;
	XMMATRIX boneScaleMatrix =
	{
		0.1f, 0, 0, 0,
		0, 0.1f, 0, 0,
		0, 0, 0.1f, 0,
		0, 0, 0, 1
	};
protected:
	UINT indexCount = 0;
	UINT vertexCount = 0;
	ID3D11Buffer* vertexBuffer = NULL;
	ID3D11Buffer* indexBuffer = NULL;
	ID3D11Buffer* constantBuffer = NULL;
	ID3D11Buffer* boneBuffer = NULL;
	ID3D11InputLayout* inputLayout = NULL;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11GeometryShader* geometryShader;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11ShaderResourceView* normalResourceView;
	ID3D11ShaderResourceView* specResourceView;
	ID3D11SamplerState* samplerState;

	ID3D11RasterizerState* p_rsSolid;
	ID3D11RasterizerState* p_rsWireframe;

	Interpolator interp, interp2;
	Blender blender;

	struct SEND_TO_OBJECT
	{
		XMFLOAT4X4 objectMatrix;
	};

	struct BONES
	{
		XMFLOAT4X4 b1, b2, b3, b4;
	};
	union BBUFFER
	{
		XMFLOAT4X4 boneOffsets[28];
		BONES bones;
	};

	SEND_TO_OBJECT worldMatrix;
	BBUFFER boneOffset;

public:

	MeshClass() {};
	~MeshClass() {};

	void Initialize(ID3D11Device* device);
	void Render(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* p_dsView, ID3D11Device* device, float delta);
	void Shutdown();
	//bool loadOBJ(const char* _path, vector<OBJ_VERT>& out_vertices, vector<UINT>& out_indices);
};