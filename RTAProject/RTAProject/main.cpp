#include <Windows.h>
#include <ctime>
#include <iostream>
#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")

#include <cmath>
using namespace std;

#define BUFFER_WIDTH	800
#define BUFFER_HEIGHT	800

HWND hWnd;

//Windows App Class
class APP
{
	HINSTANCE	h_application;
	WNDPROC		appWndProc;
	HWND		h_window;

	//Device, Swapchain, Viewport,etc
	ID3D11Device* p_device;
	ID3D11DeviceContext* p_context;
	ID3D11RenderTargetView* p_RTV;
	IDXGISwapChain* p_swapchain;
	D3D11_VIEWPORT viewport;

	//Buffers
	ID3D11Buffer* p_sceneBuffer;

	//Depth Stencil
	ID3D11Texture2D* p_depthStencil = NULL;
	ID3D11DepthStencilState* p_dsState;
	ID3D11DepthStencilView* p_dsView;
	
	ID3D11RasterizerState* p_rsSolid;
	ID3D11RasterizerState* p_rsWireframe;

public:
	APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	
};
APP* p_application;

APP::APP(HINSTANCE hinst, WNDPROC proc)
{
/////////// WINDOWS CODE
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

	//Viewport
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = sc_Descrip.BufferDesc.Width;
	viewport.Height = sc_Descrip.BufferDesc.Height;
	viewport.MaxDepth = 1.0f;

	//Scene Buffer


	p_application = this;
}

bool APP::Run()
{
	//Clear To Back Color
	const float backColor[4] = { 0, 0, 0.4, 1 };
	p_context->ClearRenderTargetView(p_RTV, backColor);
	//p_context->ClearDepthStencilView(p_dsView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Set Depth Stencil & Viewport
	//p_context->OMSetDepthStencilState(p_dsState, 1);
	//p_context->OMSetRenderTargets(1, &p_RTV, p_dsView);

	p_context->RSSetViewports(1, &viewport);
	p_context->RSSetState(p_rsSolid);

	p_swapchain->Present(0, 0);
	return true;
}


bool APP::ShutDown()
{
	UnregisterClass(L"DirectXApplication", h_application);
	return true;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
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