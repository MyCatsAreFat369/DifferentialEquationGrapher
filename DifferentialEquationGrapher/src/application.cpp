#include <application.h>

#include <stb/stb_image.h>
#include "input.h"
#include "rendering/shaderClass.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <sstream>

#include "calculator/variable.h"
#include "calculator/variableList.h"
#include "calculator/equation.h"
#include "calculator/equationList.h"



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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Text Manager
	textManager = new TextManager();

	if (!textManager->initializeTextManager())
	{
		shouldTerminate = true;
		return;
	}

	if (!textManager->loadFont("notosans", "res/Fonts/NotoSansJP-VF.ttf"))
	{
		shouldTerminate = true;
		return;
	}
	if (!textManager->loadFont("comicsans", "res/Fonts/Qdbettercomicsansbold-511d8.ttf"))
	{
		shouldTerminate = true;
		return;
	}
	if (!textManager->loadFont("jetbrainsmono", "res/Fonts/Jetbrains Mono/JetBrainsMono-Bold.ttf"))
	{
		shouldTerminate = true;
		return;
	}

	textManager->doneWithFT();

	/// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Get window width and height
	int width, height;
	glfwGetWindowSize(window->window, &width, &height);
	// set viewport
	int graphWidth = width - 400;
	int graphHeight = height - 100;

	// Grapher and calculator
	vertexShader = Shader::CompileShader(GL_VERTEX_SHADER, "res/Shaders/default.vert");
	fragmentShader = Shader::CompileShader(GL_FRAGMENT_SHADER, "res/Shaders/default.frag");

	equationList = new EquationList();
	variableList = new VariableList(equationList);

	calculator = new Calculator(equationList, variableList);

	graphManager = new GraphManager(vertexShader, fragmentShader,
									equationList, variableList,
									calculator, input,
									textManager,
									0.0f, 0.0f, 0.5f, 0.5f,
									graphWidth, graphHeight);

	Equation* someEquation = new Equation(equationList, variableList);
	someEquation->SetFormula("-10 * x");
	someEquation->setFunctionName("x");
	someEquation->derivativeOrder = 1;
	someEquation->InitializeCurve(vertexShader, fragmentShader, graphManager->x, graphManager->y, graphManager->zoomX, graphManager->zoomY);

	equationList->AddEquation(someEquation);
	variableList->addFunctionVariableIfNotExists("x");
	someEquation->Compile();

	Variable* functionVariable = variableList->getFunctionVariable("x");
	functionVariable->initialValues[0] = 1.0f;

	variableList->setVariable("kA", 19.0f);

	//std::cout << "Some equation's function name: " << someEquation->functionName << std::endl;

	graphManager->redrawCurves();

	menuGUI = new MenuGUI(equationList, variableList);
	equationGUI = new EquationGUI(equationList, variableList, graphManager);

	mySliderValue = 1.0f;
}

void Application::loop()
{
	if(shouldTerminate) return;

	double prevTime = glfwGetTime();
	double dt = 0.0;

	variableLog["time"] = (double)0.0;

	// Main loop
	while (!glfwWindowShouldClose(window->window))
	{
		double currentTime = glfwGetTime();
		dt = currentTime - prevTime;
		prevTime = currentTime;

		variableLog["time"] += dt;

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

		menuGUI->construct(width, height);

		textManager->updateTextProjection(graphWidth, graphHeight);

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

		if (input->getKeyDown(GLFW_KEY_E))
		{
			std::cout << variableLog["time"].val<double>() << std::endl;
		}

		/*
		textManager->renderFloatScientific("jetbrainsmono", 100.0f, 0.0f, height, glm::vec2(0.5f, 0.5f), glm::vec3(0.9f, 0.9f, 0.9f),
										   glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f),
										   true);
		*/
		//textManager->renderFloatScientific("jetbrainsmono", 100.0f, (float)graphWidth / 2, (float)graphHeight / 2, glm::vec2(1.0f, 1.0f), glm::vec3(1.0f),
										   //glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f),
										   //false);

		/*
		ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f));
		ImGui::Begin("My new window");
		ImGui::SliderFloatWithSteps("My Slider", &mySliderValue, -10.0f, 10.0f, 0.1f, "%.3f");
		ImGui::End();
		*/
		//std::cout << "My equation's function name: " << equationList->GetEquation(0)->functionName << std::endl;

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

	// Delete Text Manager
	textManager->Delete();
	delete textManager;

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