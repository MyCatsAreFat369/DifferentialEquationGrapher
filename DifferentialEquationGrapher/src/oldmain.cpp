/*

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

//#include <ft2build.h>
//#include FT_FREETYPE_H

#include "rendering/shaderClass.h"
#include "rendering/VAO.h"
#include "rendering/VBO.h"
#include "rendering/EBO.h"
#include "rendering/texture2d.h"
#include "grapher/curve.h"
#include "grapher/points.h"
#include "grapher/calculator.h"
#include "grapher/line.h"
#include "grapher/graphLines.h"
#include "input.h"
#include <vector>

const int SCREEN_WIDTH_DEFAULT = 1920;
const int SCREEN_HEIGHT_DEFAULT = 1080;

int main()
{
	// Initialize GLFW
	glfwInit();

	// Set version of GLFW and Profile
	// Profile just defines what methods are included into OpenGL
	// Core means we're using the modern methods
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window with a resolution x,y, and a title
	int initialWidth = 1600, initialHeight = 900;
	GLFWwindow* window = glfwCreateWindow(initialWidth, initialHeight, "Hello!", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}

	//FT_Library free_type;
	//FT_Error error_code = FT_Init_FreeType(&free_type);
	//if (error_code)
	//{
	//	std::cout << "\nError code: " << error_code << " --- An error occurred during initialization of the FT_Library.\n";
	//	int keep_console_open;
	//	std::cin >> keep_console_open;
	//}

	// make the window we created be the context for this program's GLFW
	glfwMakeContextCurrent(window);

	// Load Glad
	gladLoadGL();

	/// SCREEN RESIZING AND REPOSITIONING

	// Get screen width and height of current monitor (the one window opened in)
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	//std::cout << "I am " << currentMonitor << ". Am I a nullptr? " << (currentMonitor == nullptr) << std::endl;
	const GLFWvidmode* vidMode = glfwGetVideoMode(primaryMonitor);
	int screenWidth = vidMode->width, screenHeight = vidMode->height;
	std::cout << "Screen Size: " << screenWidth << "x" << screenHeight << std::endl;

	// scale down window size based on which screen size the app started in.
	int setWidth = initialWidth * ((double)screenWidth / SCREEN_WIDTH_DEFAULT);
	int setHeight = initialHeight * ((double)screenHeight / SCREEN_HEIGHT_DEFAULT);
	glfwSetWindowSize(window, setWidth, setHeight);
	glfwSetWindowPos(window, screenWidth / 2 - setWidth / 2, screenHeight / 2 - setHeight / 2);

	/// INPUTS

	Input::setupInputs(window);

	// im lazy so hardcode
	std::vector<int> keys;
	keys.push_back(32);
	keys.push_back(39);
	for (int i = 44; i <= 57; i++) { keys.push_back(i); }
	keys.push_back(59);
	keys.push_back(61);
	for (int i = 65; i <= 93; i++) { keys.push_back(i); }
	keys.push_back(96);
	for (int i = 256; i <= 269; i++) { keys.push_back(i); }
	for (int i = 280; i <= 284; i++) { keys.push_back(i); }
	for (int i = 290; i <= 314; i++) { keys.push_back(i); }
	for (int i = 330; i <= 336; i++) { keys.push_back(i); }
	for (int i = 340; i <= 348; i++) { keys.push_back(i); }

	// Initialize input object (you can make more later)
	Input input(keys);

	// flip the image vertically because OpenGL and stb read images differently
	stbi_set_flip_vertically_on_load(true);

	//Texture2D crepeGirlTexture("res/Textures/crepe_girl.png", GL_TEXTURE0, GL_NEAREST, GL_REPEAT, GL_RGBA, GL_UNSIGNED_BYTE);
	//crepeGirlTexture.texUnit(shaderProgram, "tex0", 0);

	GLuint vertexShader = Shader::CompileShader(GL_VERTEX_SHADER, "res/Shaders/default.vert");
	GLuint fragmentShader = Shader::CompileShader(GL_FRAGMENT_SHADER, "res/Shaders/default.frag");

	float t0 = 0.0f;
	float x0 = 1.0f, v0 = 1.0f;
	float zoomX = 1.0f, zoomY = 1.0f;

	// Initialize Curve
	Points* points = new Points();
	getPoints(points, t0, x0, v0, zoomX, zoomY, 0);
	Curve* curve = new Curve(points, zoomX, zoomY);
	curve->AttachShaders(vertexShader, fragmentShader);
	curve->x = 0.0f, curve->y = 0.0f;

	// Consider moving GraphLines creation before Curve
	GraphLines* graphLines = new GraphLines(vertexShader, fragmentShader, 0.0f, 0.0f, zoomX, zoomY);
	graphLines->UpdatePosition(0.0f, 0.0f);

	double preMouseX = 0.0, preMouseY = 0.0;
	double preCurveX = curve->x, preCurveY = curve->y;
	bool isDragging = false;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		// set viewport
		int graphWidth = width - 400;
		int graphHeight = height - 100;
		glViewport(400, 0, graphWidth, graphHeight);

		// change bg color
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/// Pan
		if (input.leftMouseButtonDown())
		{
			if (!isDragging)
			{
				preMouseX = input.getMouseX(), preMouseY = input.getMouseY();
				preCurveX = curve->x, preCurveY = curve->y;
				isDragging = true;
			}

			/// Consider making graphLines the first one to get the new position, or perhaps make the position a variable in main class
			curve->x = preCurveX + (input.getMouseX() - preMouseX) / 800.0;
			curve->y = preCurveY - (input.getMouseY() - preMouseY) / 800.0;
			graphLines->UpdatePosition(curve->x, curve->y);
		}
		else if (isDragging) isDragging = false;

		/// Zoom
		/// TODO: Make it so that zooming zooms to the mouse, not the center of the screen
		double mouseScrollY = input.getMouseScrollY();
		if (mouseScrollY != 0)
		{
			if (input.getKeyDown(GLFW_KEY_LEFT_SHIFT))
			{
				float scalingFactor = mouseScrollY / 20.0f;
				curve->scaleX += scalingFactor * curve->scaleX;
				curve->x += scalingFactor * curve->x;
				zoomX += scalingFactor * zoomX;
			}
			else
			{
				float scalingFactor = mouseScrollY / 20.0f;
				curve->scaleY += scalingFactor * curve->scaleY;
				curve->y += scalingFactor * curve->y;
				zoomY += scalingFactor * zoomY;
			}

			graphLines->UpdateZoom(zoomX, zoomY);
		}

		/// Check if we should draw new curve
		bool curveRedraw_isTooZoomed = ((curve->points->t0_right - curve->points->t0_left) * curve->scaleX >= 2 * CURVE_MAX_RADIUS_PER_WIDTH);
		bool curveRedraw_middleShows = (abs(curve->points->t0 * curve->scaleX + curve->x) <= 1.0f);
		bool curveRedraw_leftShows = (curve->points->t0_left * curve->scaleX + curve->x >= -1.0f);
		bool curveRedraw_rightShows = (curve->points->t0_right * curve->scaleX + curve->x <= 1.0);
		if ((curveRedraw_leftShows && curveRedraw_rightShows) ||
			(curveRedraw_isTooZoomed) ||
			(curveRedraw_middleShows && (curveRedraw_leftShows || curveRedraw_rightShows)))
		{
			curve = redrawCurve(curve, 0);
		}
		else if (curveRedraw_leftShows)
		{
			std::cout << "Test from the left: " << curve->points->t0_left * curve->scaleX + curve->x << std::endl;
			std::cout << "Test from the middle: " << curve->points->t0 * curve->scaleX + curve->x << std::endl;
			std::cout << "Test from the right: " << curve->points->t0_right * curve->scaleX + curve->x << std::endl;
			curve = redrawCurve(curve, -1);
		}
		else if (curveRedraw_rightShows)
		{
			std::cout << "Test from the left: " << curve->points->t0_left * curve->scaleX + curve->x << std::endl;
			std::cout << "Test from the middle: " << curve->points->t0 * curve->scaleX + curve->x << std::endl;
			std::cout << "Test from the right: " << curve->points->t0_right * curve->scaleX + curve->x << std::endl;
			curve = redrawCurve(curve, 1);
		}
		

		/// Draw graphLines
		graphLines->Draw();

		/// Draw curve
		curve->Draw();

		// switch buffers to update
		glfwSwapBuffers(window);

		// Take care of all GLFW events
		glfwPollEvents();
	}

	curve->Delete();
	delete curve;

	graphLines->Delete();
	delete graphLines;

	// Delete the shaders you created
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Destroy window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
*/


// OLD MAIN FUNCTION!