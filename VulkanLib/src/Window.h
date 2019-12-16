#pragma once
#include <Windows.h>
#include <ShellScalingApi.h> // notwendig f�r high dpi scaling

struct MotionPos {
	int x;
	int y;
};

class Window
{
protected:
	HWND window;
	HINSTANCE hInstance;
	WNDCLASS wc;
	DEVMODE screenSettings;
	const char *windowName;
	int width;
	int height;
	bool isFullScreen;
public:
	Window(const char *_windowName, int _width, int _height, bool _isFullScreen);
	~Window();
	void createWindow();
	void showWindow();
	bool* getKey();
	MotionPos* getMotionPosition();
	bool checkMessage();
	HWND getWindow();
	HINSTANCE getInstance();
	int getWidth();
	int getHeight();
};