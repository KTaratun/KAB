#include <Windows.h>
#include <ctime>
#include <iostream>

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

public:
	APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();

};
APP* p_application;

APP::APP(HINSTANCE hinst, WNDPROC proc)
{
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

	p_application = this;
}

bool APP::Run()
{
	
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