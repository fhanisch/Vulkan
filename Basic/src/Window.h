#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <ShellScalingApi.h> // notwendig für high dpi scaling

static bool key[256];
typedef struct {
	int x;
	int y;
} MousePos;

static MousePos mousePos;

class Window
{
protected:
	HWND window = nullptr;
	HINSTANCE hInstance;
	WNDCLASS wc;
	DEVMODE screenSettings = {};
	const char* windowName;
	int width;
	int height;
public:
	Window(const char* _windowName);
	~Window();
	void createWindow(int _width, int _height, bool isFullScrren);
	void showWindow();
	bool* getKey();
	MousePos* getMousePos();
	bool checkMessage();
	HWND getWindow();
	HINSTANCE getInstance();
	int getWidth();
	int getHeight();
};

