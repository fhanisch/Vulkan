#pragma once
#include <Windows.h>
#include <ShellScalingApi.h> // notwendig für high dpi scaling

class Window {
protected:
	HWND window;
	HINSTANCE hInstance;
	WNDCLASS wc;
	const char *windowName;
	int width;
	int height;
public:
	Window(const char *_windowName, int _width, int _height);
	~Window();
	void createWindow();
	void showWindow();
	bool *getKey();
	bool checkMessage();
	HWND getWindow();
	HINSTANCE getInstance();
	int getWidth();
	int getHeight();
};

