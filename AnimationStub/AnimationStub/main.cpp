#include "stdafx.h"
#include <Windows.h>
#include <ctime>

#include "cubeModel.h"
#include "boxBind.h"
#include "../FBXExporter/FBXLoader.h"

#include <cmath>
#include "xTime.h"

//using namespace DirectX;

#define BUFFER_WIDTH	800
#define BUFFER_HEIGHT	800

HWND hWnd;

//Windows App Class
class APP
{
	struct SCENE_BUFFER
	{
		XMFLOAT4X4 m4x4_view;
		XMFLOAT4X4 m4x4_projection;
	};

	struct LIGHT_BUFFER
	{
		XMFLOAT4 diffuse_ambientLight;

		XMFLOAT4 diffuse_spotLight;
		XMFLOAT4 position_spotLight;
		XMFLOAT4 direction_spotLight;
		float ratio_spotLight;
		XMFLOAT3 padding;
	};

	HINSTANCE	h_application;
	WNDPROC		appWndProc;
	HWND		h_window;

	//Device, Swapchain, Viewport,etc
	ID3D11Device* p_device;
	ID3D11DeviceContext* p_context;
	ID3D11RenderTargetView* p_RTV;
	IDXGISwapChain* p_swapchain;
	D3D11_VIEWPORT viewport;

	SCENE_BUFFER scene_matrix;
	LIGHT_BUFFER light_data;

	//Buffers
	ID3D11Buffer* p_sceneBuffer;
	ID3D11Buffer* p_lightBuffer;

	//Depth Stencil
	ID3D11Texture2D* p_depthStencil = NULL;
	ID3D11DepthStencilState* p_dsState;
	ID3D11DepthStencilView* p_dsView;

	ID3D11RasterizerState* p_rsSolid;
	ID3D11RasterizerState* p_rsWireframe;

	XMFLOAT4X4 m4x4_camera;
	XMFLOAT4X4 m4x4_camTranslate;
	XMFLOAT4X4 m4x4_Identity = { 1,0,0,0,
								 0,1,0,0,
								 0,0,1,0,
								 0,0,0,1 };

	XTime xTime;

	CubeModel* Cube;
	MeshClass* box;

	int moveSpeed;
	bool mouseMove;
	bool mouseDown;
	POINT mouse;
	POINT mouse2;
public:
	APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void MouseMove();
	void MouseNoMove();

};
APP* p_application;

void APP::MouseMove()
{
	mouseMove = true;
}
void APP::MouseNoMove()
{
	mouseMove = false;
}

APP::APP(HINSTANCE hinst, WNDPROC proc)
{
	/*_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetBreakAlloc(667);*/
	/////////// WINDOWS CODEsd
	h_application = hinst;
	appWndProc = proc;

	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = h_application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);

	RegisterClassEx(&wndClass);

	RECT wind_size = { 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT };
	AdjustWindowRect(&wind_size, WS_OVERLAPPEDWINDOW, false);

	h_window = CreateWindow(L"DirectXApplication", L"RTA Project", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, wind_size.right - wind_size.left, wind_size.bottom - wind_size.top,
		NULL, NULL, h_application, this);

	ShowWindow(h_window, SW_SHOW);

	hWnd = h_window;
	//////////////////


		//Matrices
	XMMATRIX viewMat = XMLoadFloat4x4(&scene_matrix.m4x4_view);
	viewMat = XMMatrixInverse(NULL, XMMatrixIdentity());
	XMStoreFloat4x4(&scene_matrix.m4x4_view, viewMat);
	scene_matrix.m4x4_view.m[3][2] = -2.0f;

	XMMATRIX projMat = XMLoadFloat4x4(&scene_matrix.m4x4_projection);
	projMat = XMMatrixPerspectiveFovLH(70, BUFFER_HEIGHT / BUFFER_WIDTH, 0.1f, 500);
	XMStoreFloat4x4(&scene_matrix.m4x4_projection, projMat);

	m4x4_camera = { 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,-2.0f,1 };

	m4x4_camTranslate = m4x4_Identity;

	//Lights
	light_data.diffuse_ambientLight = { 0.2f,0.2f,0.2f,1.0f };

	light_data.diffuse_spotLight = { 1,1,1,1 };
	light_data.position_spotLight = { 0,2,0,1 };
	light_data.direction_spotLight = { 0.75,-0.75f,-0.75f,1 };
	light_data.ratio_spotLight = 0.93f;

	//Swapchain Descriptor
	DXGI_SWAP_CHAIN_DESC sc_Descrip;
	ZeroMemory(&sc_Descrip, sizeof(sc_Descrip));

	sc_Descrip.BufferCount = 1;
	sc_Descrip.BufferDesc.Width = BUFFER_WIDTH;
	sc_Descrip.BufferDesc.Height = BUFFER_HEIGHT;
	sc_Descrip.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sc_Descrip.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sc_Descrip.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_Descrip.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_Descrip.OutputWindow = h_window;
	sc_Descrip.SampleDesc.Count = 1;
	sc_Descrip.SampleDesc.Quality = 0;
	sc_Descrip.Windowed = true;
	sc_Descrip.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sc_Descrip.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT flag;

	if (_DEBUG)
		flag = D3D11_CREATE_DEVICE_DEBUG;
	else
		flag = 1;

	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flag, NULL, NULL, D3D11_SDK_VERSION, &sc_Descrip, &p_swapchain, &p_device, NULL, &p_context);

	//Create Raster States
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

	p_device->CreateRasterizerState(&rs_solidDescrip, &p_rsSolid);
	p_device->CreateRasterizerState(&rs_wireframeDescrip, &p_rsWireframe);

	//Backbuffer & RTV
	ID3D11Texture2D* bBuffer;
	ZeroMemory(&bBuffer, sizeof(bBuffer));
	p_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&bBuffer);
	p_device->CreateRenderTargetView(bBuffer, NULL, &p_RTV);
	bBuffer->Release();

	//Viewport
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)sc_Descrip.BufferDesc.Width;
	viewport.Height = (FLOAT)sc_Descrip.BufferDesc.Height;
	viewport.MaxDepth = 1.0f;

	//Scene Buffer
	D3D11_BUFFER_DESC sb_Descrip;
	ZeroMemory(&sb_Descrip, sizeof(sb_Descrip));
	sb_Descrip.ByteWidth = sizeof(SCENE_BUFFER);
	sb_Descrip.Usage = D3D11_USAGE_DYNAMIC;
	sb_Descrip.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sb_Descrip.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sb_Descrip.MiscFlags = 0;
	sb_Descrip.StructureByteStride = 0;

	p_device->CreateBuffer(&sb_Descrip, NULL, &p_sceneBuffer);

	//Light Buffer
	D3D11_BUFFER_DESC lb_Descrip;
	ZeroMemory(&lb_Descrip, sizeof(lb_Descrip));
	lb_Descrip.ByteWidth = sizeof(LIGHT_BUFFER);
	lb_Descrip.Usage = D3D11_USAGE_DYNAMIC;
	lb_Descrip.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lb_Descrip.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lb_Descrip.MiscFlags = 0;
	lb_Descrip.StructureByteStride = 0;

	p_device->CreateBuffer(&lb_Descrip, NULL, &p_lightBuffer);

	//Depth Stencil
	D3D11_TEXTURE2D_DESC ds_texDescrip;
	ZeroMemory(&ds_texDescrip, sizeof(ds_texDescrip));
	ds_texDescrip.Width = BUFFER_WIDTH;
	ds_texDescrip.Height = BUFFER_HEIGHT;
	ds_texDescrip.MipLevels = 0;
	ds_texDescrip.ArraySize = 1;
	ds_texDescrip.Format = DXGI_FORMAT_D32_FLOAT;
	ds_texDescrip.SampleDesc.Count = 1;
	ds_texDescrip.SampleDesc.Quality = 0;
	ds_texDescrip.Usage = D3D11_USAGE_DEFAULT;
	ds_texDescrip.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ds_texDescrip.CPUAccessFlags = 0;

	p_device->CreateTexture2D(&ds_texDescrip, NULL, &p_depthStencil);

	D3D11_DEPTH_STENCIL_DESC ds_Descrip;
	ZeroMemory(&ds_Descrip, sizeof(ds_Descrip));

	ds_Descrip.DepthEnable = true;
	ds_Descrip.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_Descrip.DepthFunc = D3D11_COMPARISON_LESS;

	ds_Descrip.StencilEnable = true;
	ds_Descrip.StencilReadMask = 0xff;
	ds_Descrip.StencilWriteMask = 0xff;

	ds_Descrip.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_Descrip.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	ds_Descrip.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_Descrip.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ds_Descrip.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_Descrip.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	ds_Descrip.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_Descrip.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	p_device->CreateDepthStencilState(&ds_Descrip, &p_dsState);

	D3D11_DEPTH_STENCIL_VIEW_DESC ds_viewDescrip;
	ZeroMemory(&ds_viewDescrip, sizeof(ds_viewDescrip));
	ds_viewDescrip.Format = DXGI_FORMAT_D32_FLOAT;
	ds_viewDescrip.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	ds_viewDescrip.Texture2D.MipSlice = 0;

	p_device->CreateDepthStencilView(p_depthStencil, &ds_viewDescrip, &p_dsView);

	Cube = new CubeModel();
	Cube->Initialize(p_device);

	box = new MeshClass();
	box->Initialize(p_device);

	mouseMove = false;

	p_application = this;

	
}

bool APP::Run()
{
	xTime.Signal();

	//scene_matrix.m4x4_view = OrthoAffineInverse(cameraMatrix);


	//Clear To Back Color
	const float backColor[4] = { 0, 0, 0.4f, 1 };
	p_context->ClearRenderTargetView(p_RTV, backColor);
	p_context->ClearDepthStencilView(p_dsView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Set Depth Stencil & Viewport
	p_context->OMSetDepthStencilState(p_dsState, 1);
	p_context->OMSetRenderTargets(1, &p_RTV, p_dsView);

	p_context->RSSetViewports(1, &viewport);
	p_context->RSSetState(p_rsSolid);

	if (GetAsyncKeyState(VK_SHIFT))
		moveSpeed = 15;
	else
		moveSpeed = 7;



	if (GetAsyncKeyState('W'))
	{
		m4x4_camTranslate =
		{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, moveSpeed * (float)xTime.Delta(), 1 };
		XMMATRIX translateMat = XMLoadFloat4x4(&m4x4_camTranslate);
		XMMATRIX cameraMat = XMLoadFloat4x4(&m4x4_camera);

		cameraMat = XMMatrixMultiply(translateMat, cameraMat);

		XMStoreFloat4x4(&m4x4_camera, cameraMat);

	}

	if (GetAsyncKeyState('S'))
	{
		m4x4_camTranslate =
		{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, -moveSpeed * (float)xTime.Delta(), 1 };
		XMMATRIX translateMat = XMLoadFloat4x4(&m4x4_camTranslate);
		XMMATRIX cameraMat = XMLoadFloat4x4(&m4x4_camera);

		cameraMat = XMMatrixMultiply(translateMat, cameraMat);

		XMStoreFloat4x4(&m4x4_camera, cameraMat);
	}

	if (GetAsyncKeyState('A'))
	{
		m4x4_camTranslate =
		{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-moveSpeed * (float)xTime.Delta(), 0,0, 1 };
		XMMATRIX translateMat = XMLoadFloat4x4(&m4x4_camTranslate);
		XMMATRIX cameraMat = XMLoadFloat4x4(&m4x4_camera);

		cameraMat = XMMatrixMultiply(translateMat, cameraMat);

		XMStoreFloat4x4(&m4x4_camera, cameraMat);
	}

	if (GetAsyncKeyState('D'))
	{
		m4x4_camTranslate =
		{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			moveSpeed * (float)xTime.Delta(), 0,0, 1 };
		XMMATRIX translateMat = XMLoadFloat4x4(&m4x4_camTranslate);
		XMMATRIX cameraMat = XMLoadFloat4x4(&m4x4_camera);

		cameraMat = XMMatrixMultiply(translateMat, cameraMat);

		XMStoreFloat4x4(&m4x4_camera, cameraMat);
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		m4x4_camTranslate =
		{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, moveSpeed * (float)xTime.Delta(),0, 1 };
		XMMATRIX translateMat = XMLoadFloat4x4(&m4x4_camTranslate);
		XMMATRIX cameraMat = XMLoadFloat4x4(&m4x4_camera);

		cameraMat = XMMatrixMultiply(translateMat, cameraMat);

		XMStoreFloat4x4(&m4x4_camera, cameraMat);
	}

	if (GetAsyncKeyState(VK_CONTROL))
	{
		m4x4_camTranslate =
		{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, -moveSpeed * (float)xTime.Delta(),0, 1 };
		XMMATRIX translateMat = XMLoadFloat4x4(&m4x4_camTranslate);
		XMMATRIX cameraMat = XMLoadFloat4x4(&m4x4_camera);

		cameraMat = XMMatrixMultiply(translateMat, cameraMat);

		XMStoreFloat4x4(&m4x4_camera, cameraMat);
	}

	LPPOINT m = &mouse;
	GetCursorPos(m);

	if (GetAsyncKeyState(VK_RBUTTON) && !mouseDown)
		mouseDown = true;
	else if (!GetAsyncKeyState(VK_RBUTTON))
		mouseDown = false;

	if (mouseDown)
	{
		LPPOINT m2 = &mouse2;
		GetCursorPos(m2);

		XMMATRIX newcamera = XMLoadFloat4x4(&m4x4_camera);
		XMVECTOR pos = newcamera.r[3];
		newcamera.r[3] = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
		newcamera = XMMatrixRotationX(-(mouse2.y - mouse.y)*0.1f)*newcamera*XMMatrixRotationY(-(mouse2.x - mouse.x)*0.1f);
		newcamera.r[3] = pos;

		XMStoreFloat4x4(&m4x4_camera, newcamera);
	}


	D3D11_MAPPED_SUBRESOURCE map_res;
	p_context->Map(p_sceneBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &map_res);
	memcpy(map_res.pData, &scene_matrix, sizeof(SCENE_BUFFER));
	p_context->Unmap(p_sceneBuffer, 0);

	p_context->VSSetConstantBuffers(1, 1, &p_sceneBuffer);

	D3D11_MAPPED_SUBRESOURCE light_res;
	p_context->Map(p_lightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &light_res);
	memcpy(light_res.pData, &light_data, sizeof(LIGHT_BUFFER));
	p_context->Unmap(p_lightBuffer, 0);

	p_context->PSSetConstantBuffers(0, 1, &p_lightBuffer);

	XMMATRIX viewMat = XMLoadFloat4x4(&m4x4_camera);
	viewMat = XMMatrixInverse(NULL, viewMat);
	XMStoreFloat4x4(&scene_matrix.m4x4_view, viewMat);

	Cube->Render(p_context, (float)xTime.Delta());
	box->Render(p_context, p_device, (float)xTime.Delta());

	p_swapchain->Present(0, 0);
	return true;
}


bool APP::ShutDown()
{
	Cube->Shutdown();
	delete Cube;

	box->Shutdown();
	delete box;

	p_swapchain->Release();
	p_RTV->Release();
	p_device->Release();
	p_context->Release();

	p_sceneBuffer->Release();
	p_lightBuffer->Release();

	p_depthStencil->Release();
	p_dsState->Release();
	p_dsView->Release();
	p_rsSolid->Release();
	p_rsWireframe->Release();

	UnregisterClass(L"DirectXApplication", h_application);
	return true;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
bool Load()
{
	return false;
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		static DWORD frameCount = 0; ++frameCount;
		static DWORD timePast = frameCount;
		static DWORD prevCount = GetTickCount();
		if (GetTickCount() - prevCount > 1000) // only update every second
		{
			char buffer[256];
			sprintf_s(buffer, "RTA Project | FPS: %d", frameCount - timePast);
			SetWindowTextA(hWnd, buffer);
			timePast = frameCount;
			prevCount = GetTickCount();
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;


	switch (message)
	{
	case (WM_DESTROY): { PostQuitMessage(0); }
					   break;
					   //case (WM_SIZE) :
					   //	IDXGISwapChain* newSwapChain;
					   //	newSwapChain = my_App->GetSwapChain();
					   //	ID3D11RenderTargetView* newRTV = my_App->GetRTV();
					   //	ID3D11Device* newDevice = my_App->GetDevice();
					   //	ID3D11DeviceContext* newContext = my_App->GetContext();


	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}