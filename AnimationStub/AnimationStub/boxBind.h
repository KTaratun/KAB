#pragma once
#include "FBXLoader.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"
#pragma comment(lib,"d3d11.lib")
#include <vector>
#include <stdio.h>
#include "FBX_VS.csh"
#include "FBX_PS.csh"

using namespace DirectX;
using namespace std;
#define RELEASE_COM(x) {if (x){x->Release();x = nullptr;}}

class MeshClass
{
	std::vector<Mesh> meshes;
	std::vector<TransformNode> transformHierarchy;
	Animation animation;
	std::vector< unsigned int > control_point_indices;
protected:
	UINT indexCount = 0;
	UINT vertexCount = 0;
	ID3D11Buffer* vertexBuffer = NULL;
	ID3D11Buffer* indexBuffer = NULL;
	ID3D11Buffer* constantBuffer = NULL;
	ID3D11InputLayout* inputLayout = NULL;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11GeometryShader* geometryShader;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11SamplerState* samplerState;

	struct SEND_TO_OBJECT
	{
		XMFLOAT4X4 objectMatrix;
	};

	SEND_TO_OBJECT worldMatrix;

public:

	MeshClass() {};
	~MeshClass() {};

	void Initialize(ID3D11Device* device);
	void Render(ID3D11DeviceContext* deviceContext, float delta);
	void Shutdown();
	//bool loadOBJ(const char* _path, vector<OBJ_VERT>& out_vertices, vector<UINT>& out_indices);
};