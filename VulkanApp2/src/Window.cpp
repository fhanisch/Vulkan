#include "Window.h"

static bool key[256];

LRESULT CALLBACK mainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		key[wParam] = TRUE;
		break;
	case WM_KEYUP:
		key[wParam] = FALSE;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Window::Window(const char *_windowName, int _width, int _height)
{
	windowName = _windowName;
	width = _width;
	height = _height;
	hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = "Test";
	wc.lpszMenuName = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.lpfnWndProc = mainWndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	RegisterClass(&wc);
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE); // notwendig für high dpi scaling
}

Window::~Window() { if (window) DestroyWindow(window); }

void Window::createWindow()
{
	//Fullscreen
	memset(&screenSettings, 0, sizeof(screenSettings));
	screenSettings.dmSize = sizeof(screenSettings);
	screenSettings.dmPelsWidth = width;
	screenSettings.dmPelsHeight = height;
	screenSettings.dmBitsPerPel = 32;
	screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	ChangeDisplaySettings(&screenSettings, 0);

	//DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	DWORD dwStyle = WS_POPUP;

	window = CreateWindow(wc.lpszClassName, windowName, dwStyle, 0, 0, width, height, NULL, NULL, hInstance, NULL);
	ShowCursor(0);
}

void Window::showWindow()
{
	ShowWindow(window, SW_SHOW);
}

bool *Window::getKey()
{
	return &key[0];
}

bool Window::checkMessage()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return TRUE;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else {
		if (key[27]) return TRUE;
	}
	return FALSE;
}

HWND Window::getWindow() { return window; }

HINSTANCE Window::getInstance() { return hInstance; }

int Window::getWidth() { return width; }

int Window::getHeight() { return height; }