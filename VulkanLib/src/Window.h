#ifndef WINDOW_H
#define WINDOW_H

struct MotionPos {
	int xWin;
	int yWin;
	int xScreen;
	int yScreen;
};

#ifdef WINDOWS

#include <Windows.h>
#include <ShellScalingApi.h> // notwendig für high dpi scaling

class Window0
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
	Window0(const char *_windowName, int _width, int _height, bool _isFullScreen);
	~Window0();
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

#elif LINUX

#include <X11/Xlib.h>

class Window0
{
protected:
	Display *display;
	Window window;
	const char *windowName;
	int width;
	int height;
	bool isFullScreen;
public:
	bool isQuit = false;
	Window0(const char *_windowName, int _width, int _height, bool _isFullScreen);
	~Window0();
	int createWindow();
	void showWindow();
	bool* getKey();
	MotionPos* getMotionPosition();
	bool checkMessage();
	Display* getDisplay();
	Window getWindow();
	int getWidth();
	int getHeight();
};

#endif

#endif /* WINDOW_H */