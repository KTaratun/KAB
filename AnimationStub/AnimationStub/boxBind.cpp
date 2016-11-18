#include "stdafx.h"
#include "boxBind.h"
#include "FBX_VS.csh"
#include "FBX_PS.csh"
#include "Bone_VS.csh"
#include "Bone_PS.csh"

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
	rs_wireframeDescrip.FillMode = D3D11_FILL_WIREFRAME;

	device->CreateRasterizerState(&rs_solidDescrip, &p_rsSolid);
	device->CreateRasterizerState(&rs_wireframeDescrip, &p_rsWireframe);

	//FBXLoader::Load("Death.fbx", meshes, transformHierarchy, animations, texture_name);
	//Mesh mOne, mTwo;
	//Animation aOne, aTwo;
	//TransformNode tOne, tTwo;

	std::vector<TransformNode> tn;
	std::vector<Mesh> m;
	string a;

	//FBXLoader::Load("Box_Idle.fbx", meshes, transformHierarchy, animations, a); // all bones from both .fbxs are being concatenated
	//FBXLoader::Load("Box_Jump.fbx", m, tn, animations, a); // tn is literally doing nothing

	FBXLoader::Load("Idle.fbx", meshes, transformHierarchy, animations, a); // all bones from both .fbxs are being concatenated
	FBXLoader::Load("Walk.fbx", m, tn, animations, a); // tn is literally doing nothing

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
	blender.SetBones((int)transformHierarchy.size());

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

	D3D11_BUFFER_DESC boneBufferDesc;
	ZeroMemory(&boneBufferDesc, sizeof(boneBufferDesc));
	boneBufferDesc.ByteWidth = sizeof(BBUFFER);
	boneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	boneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	boneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	boneBufferDesc.MiscFlags = 0;
	boneBufferDesc.StructureByteStride = 0;

	device->CreateBuffer(&boneBufferDesc, NULL, &boneBuffer);


	//CreateDDSTextureFromFile(device, L"TestCube.dds", nullptr, &shaderResourceView);
	CreateDDSTextureFromFile(device, L"PPG_3D_Player_D.dds", nullptr, &shaderResourceView);
	CreateDDSTextureFromFile(device, L"PPG_3D_Player_N.dds", nullptr, &normalResourceView);
	CreateDDSTextureFromFile(device, L"PPG_3D_Player_spec.dds", nullptr, &specResourceView);

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

	D3D11_MAPPED_SUBRESOURCE boneRes;
	deviceContext->Map(boneBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &boneRes);
	memcpy(boneRes.pData, &boneOffset, sizeof(BBUFFER));
	deviceContext->Unmap(boneBuffer, NULL);

	deviceContext->VSSetConstantBuffers(2, 1, &boneBuffer);

	keyFrame = blender.Update(delta, meshes[0].verts, transformHierarchy);
	std::vector<Vertex> vertOut = meshes[0].verts;
	for (UINT i = 0; i < transformHierarchy.size(); i++)
	{
		XMStoreFloat4x4(&boneOffset.boneOffsets[i], blender.GetSkinningMatrix()[i]);
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	deviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
	deviceContext->PSSetShaderResources(1, 1, &normalResourceView);
	deviceContext->PSSetShaderResources(2, 1, &specResourceView);


	deviceContext->PSSetSamplers(0, 1, &samplerState);
	deviceContext->PSSetSamplers(1, 1, &samplerState);


	deviceContext->VSSetShader(vertexShader, NULL, NULL);
	deviceContext->PSSetShader(pixelShader, NULL, NULL);

	deviceContext->IASetInputLayout(inputLayout);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//deviceContext->RSSetState(p_rsWireframe);
	deviceContext->Draw((UINT)meshes[0].verts.size(), 0);
	deviceContext->RSSetState(p_rsSolid);
	for (UINT i = 0; i < keyFrame.bones.size(); i++)
		XMStoreFloat4x4(&boneSpheres[i]->worldMatrix.objectMatrix, XMMatrixMultiply(boneScaleMatrix, keyFrame.bones[i]));
	// RENDER BONES
	for (unsigned int i = 0; i < boneSpheres.size(); i++)
		boneSpheres[i]->Render(deviceContext, delta);

}

void MeshClass::Shutdown()
{
	RELEASE_COM(vertexBuffer);
	RELEASE_COM(indexBuffer);
	RELEASE_COM(constantBuffer);
	RELEASE_COM(inputLayout);
	RELEASE_COM(vertexShader);
	RELEASE_COM(pixelShader);
	//	RELEASE_COM(texture);
	RELEASE_COM(shaderResourceView);
	RELEASE_COM(samplerState);

	for (unsigned int i = 0; i < boneSpheres.size(); i++)
	{
		boneSpheres[i]->Shutdown();
		delete boneSpheres[i];
	}

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

