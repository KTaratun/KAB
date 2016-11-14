#include "modelClass.h"



class CubeModel : public ModelClass
{

public:
	CubeModel() {};
	~CubeModel() {};

	void Initialize(ID3D11Device* device);
	void Render(ID3D11DeviceContext* deviceContext, float delta);
	void Shutdown();
};