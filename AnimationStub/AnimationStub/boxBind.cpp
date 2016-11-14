#include "stdafx.h"
#include "boxBind.h"
#include "FBX_VS.csh"
#include "FBX_PS.csh"
#include "Bone_VS.csh"
#include "Bone_PS.csh"
#define DIFFUSE 1
#define SPECULAR 0
#define NORMAL 0

void MeshClass::Initialize(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC rs_solidDescrip;
	rs_solidDescrip.AntialiasedLineEnable = false;
	rs_solidDescrip.CullMode = D3D11_CULL_BACK;
	rs_solidDescrip.DepthBias = 0;
	rs_solidDescrip.DepthBiasClamp = 0.0f;
	rs_solidDescrip.DepthClipEnable = true;
	rs_solidDescrip.FillMode = D3D11_FILL_SOLID;
	rs_solidDescrip.FrontCounterClockwise = false;
	rs_solidDescrip.MultisampleEnable = false;
	rs_solidDescrip.ScissorEnable = false;
	rs_solidDescrip.SlopeScaledDepthBias = 0.0f;

	D3D11_RASTERIZER_DESC rs_wireframeDescrip = rs_solidDescrip;
	//rs_wireframeDescrip.FillMode = D3D11_FILL_WIREFRAME;

	device->CreateRasterizerState(&rs_solidDescrip, &p_rsSolid);
	device->CreateRasterizerState(&rs_wireframeDescrip, &p_rsWireframe);
	
	//FBXLoader::Load("Death.fbx", meshes, transformHierarchy, animations, texture_name);
	//Mesh mOne, mTwo;
	//Animation aOne, aTwo;
	//TransformNode tOne, tTwo;

	std::vector<TransformNode> tn;
	std::vector<Mesh> m;
	string a;

	FBXLoader::Load("Box_Idle.fbx", meshes, transformHierarchy, animations, a); // all bones from both .fbxs are being concatenated
	FBXLoader::Load("Box_Jump.fbx", m, tn, animations, a); // tn is literally doing nothing
	
	for (UINT i = 0; i < transformHierarchy.size(); i++)
	{
		boneMatrices.push_back(transformHierarchy[i].GetLocal());
	}
	XMFLOAT4X4 IdentityMatrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	interp.SetAnimPtr(&animations[0]);
	interp2.SetAnimPtr(&animations[1]);
	blender.SetAnim(&interp);
	blender.SetNextAnim(&interp2);
	blender.SetBones(transformHierarchy.size());

	worldMatrix.objectMatrix = IdentityMatrix;

	HRESULT hr;

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = NULL;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * (UINT)meshes[0].verts.size();
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialDataVertex;
	initialDataVertex.pSysMem = meshes[0].verts.data();

	hr = device->CreateBuffer(&vertexBufferDesc, &initialDataVertex, &vertexBuffer);

	device->CreateVertexShader(FBX_VS, sizeof(FBX_VS), NULL, &vertexShader);
	device->CreatePixelShader(FBX_PS, sizeof(FBX_PS), NULL, &pixelShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(layout, ARRAYSIZE(layout), FBX_VS, sizeof(FBX_VS), &inputLayout);
	D3D11_BUFFER_DESC objectConstantBufferDesc;
	ZeroMemory(&objectConstantBufferDesc, sizeof(objectConstantBufferDesc));
	objectConstantBufferDesc.ByteWidth = sizeof(SEND_TO_OBJECT);
	objectConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectConstantBufferDesc.MiscFlags = 0;
	objectConstantBufferDesc.StructureByteStride = 0;

	device->CreateBuffer(&objectConstantBufferDesc, NULL, &constantBuffer);

	CreateDDSTextureFromFile(device, L"TestCube.dds", nullptr, &shaderResourceView);
	//const wchar_t* tex = (const wchar_t*)texture_name.c_str();
	//CreateWICTextureFromFile(device, tex, nullptr, &shaderResourceView);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;

	device->CreateSamplerState(&samplerDesc, &samplerState);

	XMFLOAT4X4 ScaleMatrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	XMFLOAT4X4 TranslateMatrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

	XMMATRIX scaleMat = XMLoadFloat4x4(&ScaleMatrix);
	XMMATRIX translateMat = XMLoadFloat4x4(&TranslateMatrix);
	XMMATRIX objMat = XMLoadFloat4x4(&worldMatrix.objectMatrix);

	objMat = XMMatrixMultiply(translateMat, objMat);
	objMat = XMMatrixMultiply(scaleMat, objMat);

	XMStoreFloat4x4(&worldMatrix.objectMatrix, objMat);
	
	for (UINT i = 0; i < boneMatrices.size(); i++)
	{
		newboneSphere = new BoneSphere;
		newboneSphere->Initialize(device, boneMatrices[i]);
		boneSpheres.push_back(newboneSphere);
	}
		//delete newboneSphere;

}

void MeshClass::Render(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* p_dsView, ID3D11Device* device, float delta)
{
	D3D11_MAPPED_SUBRESOURCE mapRes;
	deviceContext->Map(constantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, &worldMatrix, sizeof(SEND_TO_OBJECT));
	deviceContext->Unmap(constantBuffer, NULL);

	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	deviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
	deviceContext->PSSetSamplers(0, 1, &samplerState);
	deviceContext->PSSetSamplers(1, 1, &samplerState);


	deviceContext->VSSetShader(vertexShader, NULL, NULL);
	deviceContext->PSSetShader(pixelShader, NULL, NULL);

	deviceContext->IASetInputLayout(inputLayout);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->RSSetState(p_rsWireframe);
	deviceContext->Draw((UINT)meshes[0].verts.size(), 0);
	deviceContext->RSSetState(p_rsSolid);

	blender.Update(delta, meshes[0].verts, transformHierarchy);
	std::vector<Vertex> vertOut = meshes[0].verts;

	// ROUGHLY MATH FOR SMOOTH SKINNING
	for (size_t i = 0; i < vertOut.size(); i++)
	{
		XMVECTOR vertPos = XMVectorSet(meshes[0].verts[i].xyz.x, meshes[0].verts[i].xyz.y, meshes[0].verts[i].xyz.z, 1.f);// = XMLoadFloat3(&meshes[0].verts[i].xyz);
		XMVECTOR tempVert;
		tempVert = XMVector4Transform(vertPos, blender.GetSkinningMatrix(vertOut[i].bone.x) * meshes[0].verts[i].weights.x);
		tempVert += XMVector4Transform(vertPos, blender.GetSkinningMatrix(vertOut[i].bone.y) * meshes[0].verts[i].weights.y);
		tempVert += XMVector4Transform(vertPos, blender.GetSkinningMatrix(vertOut[i].bone.z) * meshes[0].verts[i].weights.z);
		tempVert += XMVector4Transform(vertPos, blender.GetSkinningMatrix(vertOut[i].bone.w) * meshes[0].verts[i].weights.w);
		
		XMStoreFloat3(&vertOut[i].xyz, tempVert);
	}

	// RESETTING VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = NULL;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * (UINT)meshes[0].verts.size();
	vertexBufferDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA initialDataVertex;
	initialDataVertex.pSysMem = vertOut.data();
	
	HRESULT hr;
	hr = device->CreateBuffer(&vertexBufferDesc, &initialDataVertex, &vertexBuffer);

	// RENDER BONES
	for (unsigned int i = 0; i < boneSpheres.size(); i++)
		boneSpheres[i]->Render(deviceContext, delta);
}

void MeshClass::Shutdown()
{
	for (unsigned int i = 0; i < boneSpheres.size(); i++)
	{
		boneSpheres[i]->Shutdown();
		delete boneSpheres[i];
	}
	RELEASE_COM(vertexBuffer);
	RELEASE_COM(indexBuffer);
	RELEASE_COM(constantBuffer);
	RELEASE_COM(inputLayout);
	RELEASE_COM(vertexShader);
	RELEASE_COM(pixelShader);
//	RELEASE_COM(texture);
	RELEASE_COM(shaderResourceView);
	RELEASE_COM(samplerState);

//	p_dsView->Release();
	p_rsSolid->Release();
	p_rsWireframe->Release();
}


void BoneSphere::Initialize(ID3D11Device* device, XMMATRIX matrix)
{
	vertexShader = NULL;
	pixelShader = NULL;
	XMFLOAT4X4 IdentityMatrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	XMStoreFloat4x4(&worldMatrix.objectMatrix, matrix);

	vector<OBJ_VERT> sphere;
	vector<UINT> sphereIndices;
	bool res = loadOBJ("sphere.obj", sphere, sphereIndices);

	HRESULT hr;

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = NULL;
	vertexBufferDesc.ByteWidth = sizeof(OBJ_VERT) * vertexCount;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = sphere.data();

	hr = device->CreateBuffer(&vertexBufferDesc, &initialData, &vertexBuffer);

	device->CreateVertexShader(Bone_VS, sizeof(Bone_VS), NULL, &vertexShader);
	device->CreatePixelShader(Bone_PS, sizeof(Bone_PS), NULL, &pixelShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(layout, ARRAYSIZE(layout), Bone_VS, sizeof(Bone_VS), &inputLayout);

	D3D11_BUFFER_DESC objectConstantBufferDesc;
	ZeroMemory(&objectConstantBufferDesc, sizeof(objectConstantBufferDesc));
	objectConstantBufferDesc.ByteWidth = sizeof(SEND_TO_OBJECT);
	objectConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectConstantBufferDesc.MiscFlags = 0;
	objectConstantBufferDesc.StructureByteStride = 0;

	device->CreateBuffer(&objectConstantBufferDesc, NULL, &constantBuffer);

	CreateDDSTextureFromFile(device, L"moon.dds", nullptr, &shaderResourceView);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;

	device->CreateSamplerState(&samplerDesc, &samplerState);

	XMFLOAT4X4 ScaleMatrix =
	{
		0.25f, 0, 0, 0,
		0, 0.25f, 0, 0,
		0, 0, 0.25f, 0,
		0, 0, 0, 1
	};
	XMFLOAT4X4 TranslateMatrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

	XMMATRIX scaleMat = XMLoadFloat4x4(&ScaleMatrix);
	XMMATRIX translateMat = XMLoadFloat4x4(&TranslateMatrix);
	XMMATRIX objMat = XMLoadFloat4x4(&worldMatrix.objectMatrix);

	objMat = XMMatrixMultiply(translateMat, objMat);
	objMat = XMMatrixMultiply(scaleMat, objMat);

	XMStoreFloat4x4(&worldMatrix.objectMatrix, objMat);
}

void BoneSphere::Render(ID3D11DeviceContext* deviceContext, float delta)
{
	D3D11_MAPPED_SUBRESOURCE mapRes;
	deviceContext->Map(constantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, &worldMatrix, sizeof(SEND_TO_OBJECT));
	deviceContext->Unmap(constantBuffer, NULL);

	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	UINT stride = sizeof(OBJ_VERT);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	deviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	deviceContext->VSSetShader(vertexShader, NULL, NULL);
	deviceContext->PSSetShader(pixelShader, NULL, NULL);

	deviceContext->IASetInputLayout(inputLayout);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->Draw(vertexCount, 0);
}

void BoneSphere::Shutdown()
{
	RELEASE_COM(vertexBuffer);
	RELEASE_COM(indexBuffer);
	RELEASE_COM(constantBuffer);
	RELEASE_COM(inputLayout);
	RELEASE_COM(vertexShader);
	RELEASE_COM(pixelShader);
	//RELEASE_COM(texture);
	RELEASE_COM(shaderResourceView);
	RELEASE_COM(samplerState);
}