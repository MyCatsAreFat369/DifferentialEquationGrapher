#include <window.h>

#include <iostream>


Window::Window(int initialWidth, int initialHeight)
{
	// Create a window with a resolution x,y, and a title
	window = glfwCreateWindow(initialWidth, initialHeight, "Hello!", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		shouldTerminate = true;
		return;
	}

	// make the window we created be the context for this program's GLFW
	glfwMakeContextCurrent(window);

	/// SCREEN RESIZING AND REPOSITIONING

	// Get screen width and height of current monitor (the one window opened in)
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* vidMode = glfwGetVideoMode(primaryMonitor);
	int screenWidth = vidMode->width, screenHeight = vidMode->height;

	// scale down window size based on which screen size the app started in.
	int setWidth = initialWidth * ((double)screenWidth / SCREEN_WIDTH_DEFAULT);
	int setHeight = initialHeight * ((double)screenHeight / SCREEN_HEIGHT_DEFAULT);
	glfwSetWindowSize(window, setWidth, setHeight);
	glfwSetWindowPos(window, screenWidth / 2 - setWidth / 2, screenHeight / 2 - setHeight / 2);
}

void Window::Delete()
{
	// Destroy window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}