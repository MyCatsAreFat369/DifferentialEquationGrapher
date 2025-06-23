/*#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h";
#include "texture2d.h"
//#include "grapher/curve.h"

// Vertices coordinates
GLfloat oldvertices[] =
{ //               COORDINATES                  /     COLORS           //
	-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
	 0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
	-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
	 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
	 0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
};

// Indices for vertices order
GLuint oldindices[] =
{
	0, 3, 5, // Lower left triangle
	3, 2, 4, // Lower right triangle
	5, 4, 1 // Upper triangle
};

int oldmain()
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
	GLFWwindow* window = glfwCreateWindow(800, 800, "Hello!", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}
	// make the window we created be the context for this program's GLFW
	glfwMakeContextCurrent(window);

	// Load Glad
	gladLoadGL();

	// set viewport to the whole window
	glViewport(0, 0, 800, 800);
	// flip the image vertically because OpenGL and stb read images differently
	//stbi_set_flip_vertically_on_load(true);

	//Texture2D crepeGirlTexture("res/Textures/crepe_girl.png", GL_TEXTURE0, GL_NEAREST, GL_REPEAT, GL_RGBA, GL_UNSIGNED_BYTE);
	//crepeGirlTexture.texUnit(shaderProgram, "tex0", 0);

	GLfloat points[CURVE_POINTS_SIZE * 6];
	float zoomX = 1.0f, zoomY = 1.0f;
	float dt = (2 * zoomX) / (CURVE_POINTS_SIZE - 1);
	float x0 = 1.0f;
	float v0 = 0.0f;

	float x = x0;
	float v = v0;
	float a;
	float time = 0.0f;
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i < CURVE_POINTS_SIZE; i++)
	{
		std::cout << i << ": " << time * zoomX << ", " << x * zoomY << std::endl;
		points[i * 6] = time * zoomX;
		points[i * 6 + 1] = x * zoomY;
		points[i * 6 + 2] = 0.0f;
		points[i * 6 + 3] = 1.0f;
		points[i * 6 + 4] = 1.0f;
		points[i * 6 + 5] = 1.0f;

		a = -100.0f * x;
		v += a * dt;
		x += v * dt;
		time += dt;

		//std::cout << i << " " << time << std::endl;
	}
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i > 0;)
	{
		i--;
		a = -100.0f * x;
		v -= a * dt;
		x -= v * dt;
		time -= dt;

		points[i * 6] = time * zoomX;
		points[i * 6 + 1] = x * zoomY;
		points[i * 6 + 2] = 0.0f;
		points[i * 6 + 3] = 1.0f;
		points[i * 6 + 4] = 1.0f;
		points[i * 6 + 5] = 1.0f;
	}
	GLfloat verticesCurve[] =
	{
		0.0f, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		0.1f, 0.1f, 0.0f,		1.0f, 1.0f, 1.0f,
		0.2f, 0.4f, 0.0f,		1.0f, 1.0f, 1.0f,
		0.3f, 0.9f, 0.0f,		1.0f, 1.0f, 1.0f,
		0.31f, 0.91f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.32f, 0.92f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.33f, 0.93f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.34f, 0.94f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.35f, 0.95f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.36f, 0.96f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.37f, 0.97f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.38f, 0.98f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.39f, 0.99f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.40f, 1.00f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.50f, 0.90f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.60f, 0.80f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.70f, 0.70f, 0.0f,     1.0f, 1.0f, 1.0f,
		0.80f, 0.60f, 0.0f,     1.0f, 1.0f, 1.0f
	};
	GLuint indicesCurve[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 6,
		6, 7,
		7, 8,
		8, 9,
		9, 10,
		10, 11,
		11, 12,
		12, 13,
		13, 14,
		14, 15,
		15, 16,
		16, 17
	};

	// Create Shader Program for vertex and fragment shaders
	Shader shaderProgram("res/Shaders/default.vert", "res/Shaders/default.frag");
	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	VAO VAOcurve;
	VAOcurve.Bind();

	VBO VBOcurve(verticesCurve, sizeof(verticesCurve));
	EBO EBOcurve(indicesCurve, sizeof(indicesCurve));

	VAOcurve.LinkAttrib(VBOcurve, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAOcurve.LinkAttrib(VBOcurve, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAOcurve.Unbind();
	VBOcurve.Unbind();
	EBOcurve.Unbind();
	//Curve curve(verticesCurve, sizeof(verticesCurve));
	//curve.SetZoomX(zoomX);
	//curve.SetZoomY(zoomY);

	// Generates Shader object using shaders defualt.vert and default.frag
	Shader shaderProgram("res/Shaders/default.vert", "res/Shaders/default.frag");

	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();
	std::cout << "Once was " << VAO1.ID << std::endl;

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(oldvertices, sizeof(oldvertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(oldindices, sizeof(oldindices));

	std::cout << VBO1.ID << " " << EBO1.ID << std::endl;
	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// change bg color
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 1.0f);
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		// switch buffers to update
		glfwSwapBuffers(window);

		// Take care of all GLFW events
		glfwPollEvents();
	}

	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

	// Destroy window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}*/