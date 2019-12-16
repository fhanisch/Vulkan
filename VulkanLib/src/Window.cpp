#include "Window.h"

static bool key[256];
static MotionPos motionPos;

#ifdef WINDOWS
#include <windowsx.h>

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
	case WM_MOUSEMOVE:
		motionPos.x = GET_X_LPARAM(lParam);
		motionPos.y = GET_Y_LPARAM(lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Window::Window(const char *_windowName, int _width, int _height, bool _isFullScreen)
{
	windowName = _windowName;
	width = _width;
	height = _height;
	isFullScreen = _isFullScreen;
	memset(key, 0, sizeof(key));
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
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE); // notwendig f�r high dpi scaling
}

Window::~Window()
{ 
	printf("Close Window.\n");
	if (window) DestroyWindow(window);
}

void Window::createWindow()
{
	DWORD dwStyle;
	
	if (isFullScreen)
	{
		memset(&screenSettings, 0, sizeof(screenSettings));
		screenSettings.dmSize = sizeof(screenSettings);
		screenSettings.dmPelsWidth = width;
		screenSettings.dmPelsHeight = height;
		screenSettings.dmBitsPerPel = 32;
		screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&screenSettings, 0);

		dwStyle = WS_POPUP;
	}
	else
	{
		dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	}
	
	window = CreateWindow(wc.lpszClassName, windowName, dwStyle, 0, 0, width, height, NULL, NULL, hInstance, NULL);

	if (isFullScreen) ShowCursor(0);
}

void Window::showWindow()
{
	ShowWindow(window, SW_SHOW);
}

bool *Window::getKey()
{
	return &key[0];
}

MotionPos* Window::getMotionPosition()
{
	return &motionPos;
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
		if (key[VK_ESCAPE]) return TRUE;
	}
	return FALSE;
}

HWND Window::getWindow() { return window; }

HINSTANCE Window::getInstance() { return hInstance; }

int Window::getWidth() { return width; }

int Window::getHeight() { return height; }

#elif LINUX

#include <stdio.h>
#include <string.h>
#include <pthread.h>

void eventHandler(void* args)
{
    Window0* xWin = (Window0*)args;
    XEvent e;

    while (1) {
        XNextEvent(xWin->getDisplay(), &e);
        if (e.type == Expose) {
            
        }
        else if (e.type == KeyPress)
        {
            printf("KeyPress Event: %x\n", e.xkey.keycode);
            key[e.xkey.keycode] = true;
            if (key[0x9]) break;
        }
        else if (e.type == KeyRelease)
        {
            printf("KeyRelease Event: %x\n", e.xkey.keycode);
            key[e.xkey.keycode] = false;
        }
        else if (e.type == MotionNotify)
        {
            motionPos.x = e.xmotion.x;
            motionPos.y = e.xmotion.y;
        }
    }
	xWin->isQuit = true;
}

Window0::Window0(const char *_windowName, int _width, int _height, bool _isFullScreen)
{
	windowName = _windowName;
	width = _width;
	height = _height;
	isFullScreen = _isFullScreen;
	memset(key, 0, sizeof(key));
}

Window0::~Window0()
{
	printf("Close Window.\n");
	XDestroyWindow(display, window);
    XCloseDisplay(display);
}

int Window0::createWindow()
{
	int s;

	display = XOpenDisplay(NULL);
	if (display == NULL) return 1;

	s = DefaultScreen(display);
	window = XCreateSimpleWindow(display, RootWindow(display, s), 10, 10, width, height, 1, BlackPixel(display, s), WhitePixel(display, s));
	XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask);
	XStoreName(display, window, windowName);

	pthread_t threadID;
    pthread_create(&threadID, NULL, (void *(*)(void *))eventHandler, this);

	return 0;
}

void Window0::showWindow()
{
	XMapWindow(display, window);
}

bool Window0::checkMessage() { return isQuit; }

Display* Window0::getDisplay() { return display; }

Window Window0::getWindow() { return window; }

bool *Window0::getKey()
{
	return &key[0];
}

MotionPos* Window0::getMotionPosition()
{
	return &motionPos;
}

#endif