#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#include <GLFW/glfw3.h>

const int SCREEN_WIDTH_DEFAULT = 1920;
const int SCREEN_HEIGHT_DEFAULT = 1080;

class Window
{
	public:
		Window(int initialWidth, int initialHeight);

		void Delete();

		bool shouldTerminate = false;

		GLFWwindow* window;
};



#endif