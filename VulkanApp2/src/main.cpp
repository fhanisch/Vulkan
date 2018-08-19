/*
	Vulkan App 2.0
	19.08.2018
	FH

	Build:
		clang-cl main.cpp Window.cpp user32.lib Shcore.lib -o VulkanApp2a.exe
		cl /nologo /EHsc main.cpp Window.cpp /link user32.lib Shcore.lib /out:VulkanApp2b.exe
*/

#include <iostream>
#include "Window.h"

class App {
	// Main Application Class
	Window *window;
public:
	App() {
		std::cout << "***** HalliHalllo *****" << std::endl;
		window = new Window("Vulkan App 2", 1000, 1000);
		window->createWindow();
	}
	~App() {
		std::cout << "Servus!" << std::endl;
		delete window;
	}
	void run() {
		//bool *key = window->getKey(); --> später zur Steuerung notwendig

		window->showWindow();
		while (!window->checkMessage()) {
			
		}
	}
};

//Entry Function
int main(int argc, char *argv[]) {
	App app;
	app.run();
	return 0;
}