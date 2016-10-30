#include "modelClass.h"

void ModelClass::Initialize(ID3D11Device* device)
{

}

void ModelClass::Render(ID3D11DeviceContext* deviceContext, float delta)
{

}

void ModelClass::Shutdown()
{

}

bool ModelClass::loadOBJ(const char* _path, vector<OBJ_VERT>& out_vertices, vector<UINT>& out_indices)
{
	vector<UINT> vertIndices, uvIndices, normIndices;
	vector<XMFLOAT3> temp_verts;
	vector<XMFLOAT2> temp_uvs;
	vector<XMFLOAT3> temp_norms;
	FILE * file = fopen(_path, "r");

	if (file == NULL)
	{
		printf("Cannot Open File\n");
		return false;
	}

	while (1){
		char lineHead[128];

		int res= fscanf(file, "%s", lineHead);
		if (res == EOF)
			break;

		if (strcmp(lineHead, "v") == 0)
		{
			XMFLOAT3 vert;
			fscanf(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
			temp_verts.push_back(vert);
		}
		else if (strcmp(lineHead, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHead, "vn") == 0)
		{
			XMFLOAT3 norm;
			fscanf(file, "%f %f %f\n", &norm.x, &norm.y, &norm.z);
			temp_norms.push_back(norm);
		}
		else if (strcmp(lineHead, "f") == 0)
		{
			string vert1, vert2, vert3;
			UINT vertIndex[3], uvIndex[3], normIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertIndex[0], &uvIndex[0], &normIndex[0], &vertIndex[1], &uvIndex[1], &normIndex[1], &vertIndex[2], &uvIndex[2], &normIndex[2]);
			if (matches != 9)
			{
				printf("File can't be read. Try exporting with other options\n");
				return false;
			}
			vertIndices.push_back(vertIndex[0]);
			vertIndices.push_back(vertIndex[1]);
			vertIndices.push_back(vertIndex[2]);

			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);

			normIndices.push_back(normIndex[0]);
			normIndices.push_back(normIndex[1]);
			normIndices.push_back(normIndex[2]);

		}
	}

	for (UINT i = 0; i < vertIndices.size(); i++)
	{
		OBJ_VERT vert;
		UINT vertIndex = vertIndices[i];
		vert.pos = temp_verts[vertIndex - 1];
		
		UINT uvIndex = uvIndices[i];
		vert.uv = temp_uvs[uvIndex - 1];

		UINT normIndex = normIndices[i];
		vert.norm = temp_norms[normIndex - 1];

		out_indices.push_back(i);
		indexCount++;

		out_vertices.push_back(vert);
		vertexCount++;

	}

	return true;
}