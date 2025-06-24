#ifndef APPLICATION_CLASS_H
#define APPLICATION_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.h"

#include "grapher/graphManager.h"
#include "calculator/equationGUI.h"
#include "calculator/equation.h"
#include "calculator/equationList.h"
#include "calculator/variableList.h"

class Application
{
	public:
		Application();

		void loop();

		void Delete();
	private:
		Window* window;
		Input* input;

		GraphManager* graphManager;
		EquationGUI* equationGUI;

		EquationList* equationList;
		VariableList* variableList;

		bool* keepWindowActive;
		bool* keepLoadingWindowActive;
		std::vector<char*> names;

		bool shouldTerminate = false;

		GLuint vertexShader, fragmentShader;
};


#endif