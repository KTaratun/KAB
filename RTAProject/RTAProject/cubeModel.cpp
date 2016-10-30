#include "cubeModel.h"

void CubeModel::Initialize(ID3D11Device* device)
{
	XMFLOAT4X4 IdentityMatrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	worldMatrix.objectMatrix = IdentityMatrix;

	vector<OBJ_VERT> cube;
	vector<UINT> cubeIndices;
	bool res = loadOBJ("my_cube.obj", cube, cubeIndices);

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
	initialData.pSysMem = cube.data();

	hr = device->CreateBuffer(&vertexBufferDesc, &initialData, &vertexBuffer);

	device->CreateVertexShader(OBJ_VS, sizeof(OBJ_VS), NULL, &vertexShader);
	device->CreatePixelShader(OBJ_PS, sizeof(OBJ_PS), NULL, &pixelShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(layout, ARRAYSIZE(layout), OBJ_VS, sizeof(OBJ_VS), &inputLayout);

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
		25, 0, 0, 0,
		0, 0.05f, 0, 0,
		0, 0, 25, 0,
		0, 0, 0, 1
	};
	XMFLOAT4X4 TranslateMatrix =
	{ 1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, -1, 0, 1 };

	XMMATRIX scaleMat = XMLoadFloat4x4(&ScaleMatrix);
	XMMATRIX translateMat = XMLoadFloat4x4(&TranslateMatrix);
	XMMATRIX objMat = XMLoadFloat4x4(&worldMatrix.objectMatrix);

	objMat = XMMatrixMultiply(translateMat, objMat);
	objMat = XMMatrixMultiply(scaleMat, objMat);

	XMStoreFloat4x4(&worldMatrix.objectMatrix, objMat);
}

void CubeModel::Render(ID3D11DeviceContext* deviceContext, float delta)
{
	//indexBuffer = NULL;
	//worldMatrix.objectMatrix = MatrixMultiply_4x4(worldMatrix.objectMatrix, Rotate_X4x4(delta*5.0f));


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
	deviceContext->PSSetSamplers(1, 1, &samplerState);


	deviceContext->VSSetShader(vertexShader, NULL, NULL);
	deviceContext->PSSetShader(pixelShader, NULL, NULL);

	deviceContext->IASetInputLayout(inputLayout);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->Draw(vertexCount, 0);
}

void CubeModel::Shutdown()
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

}