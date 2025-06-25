#include <application.h>

#include <stb/stb_image.h>
#include "input.h"
#include "rendering/shaderClass.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <sstream>

Application::Application()
{
	// Initialize GLFW
	glfwInit();

	// Set version of GLFW and Profile
	// Profile just defines what methods are included into OpenGL
	// Core means we're using the modern methods
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = new Window(1600, 900);
	if (window->shouldTerminate)
	{
		shouldTerminate = true;
		return;
	}

	// Load Glad
	gladLoadGL();

	/// INPUTS

	Input::setupInputs(window->window);

	// Initialize input object (you can make more later)
	input = new Input();

	// flip the image vertically because OpenGL and stb read images differently
	stbi_set_flip_vertically_on_load(true);


	/// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	vertexShader = Shader::CompileShader(GL_VERTEX_SHADER, "res/Shaders/default.vert");
	fragmentShader = Shader::CompileShader(GL_FRAGMENT_SHADER, "res/Shaders/default.frag");

	graphManager = new GraphManager(vertexShader, fragmentShader, input,
									0.0f, 0.0f, 1.0f, 1.0f,
									1.0f, 0.0f);

	equationList = new EquationList();
	variableList = new VariableList();

	Equation* someEquation = new Equation();
	someEquation->SetFormula("-10 * x");
	equationList->AddEquation(someEquation);
	std::cout << "My beautiful equation is: " << someEquation->formula << ", " << someEquation->formulaChar << std::endl;

	variableList->setVariable("kA", 19.0f);

	equationGUI = new EquationGUI(equationList, variableList, graphManager);
}

void Application::loop()
{
	if(shouldTerminate) return;

	// Main loop
	while (!glfwWindowShouldClose(window->window))
	{
		// change bg color
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Get window width and height
		int width, height;
		glfwGetWindowSize(window->window, &width, &height);
		// set viewport
		int graphWidth = width - 400;
		int graphHeight = height - 100;
		glViewport(400, 0, graphWidth, graphHeight);

		if (!graphManager->graphLines->allLinesLoaded())
		{
			graphManager->graphLines->loadNextLines();
			float progress = graphManager->graphLines->linesLoaded / (float)graphManager->graphLines->linesToLoad();

			int windowWidth = 550.0f, windowHeight = 50.0f;
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_NoScrollbar;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
			keepLoadingWindowActive = NULL;
			ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
			ImGui::SetNextWindowPos(ImVec2(width / 2 - windowWidth / 2, height / 2 - windowHeight / 2));
			ImGui::Begin("Loading Application...", keepLoadingWindowActive, window_flags);
			ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
			ImGui::SameLine();
			ImGui::Text("loading in Line objects");
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// change to render window
			// switch buffers to update
			glfwSwapBuffers(window->window);

			// Take care of all GLFW events
			glfwPollEvents();
			continue;
		}

		equationGUI->construct(width, height);

		ImGui::ShowDemoWindow();

		graphManager->render(graphWidth, graphHeight, equationGUI->isHoveringImGui);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// change to render window
		// switch buffers to update
		glfwSwapBuffers(window->window);

		// Take care of all GLFW events
		glfwPollEvents();
	}
}


void Application::Delete()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	// Destroy input object
	delete input;

	// Destroy graph
	graphManager->Delete();
	delete graphManager;

	// Delete equation list
	equationList->Delete();
	delete equationList;

	// Delete the shaders you created
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Destroy window
	window->Delete();
	delete window;
}