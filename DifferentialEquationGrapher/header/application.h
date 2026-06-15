#ifndef APPLICATION_CLASS_H
#define APPLICATION_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.h"

#include "grapher/graphManager.h"

#include "gui/menuGUI.h"
#include "gui/equationGUI.h"

#include "grapher/calculator.h"

#include "rendering/textManager.h"

#include <jsoncpp/json.hpp>


class Application
{
	public:
		Application();

		void loop();

		void Delete();

		jsoncpp::json variableLog;

		
	private:
		Window* window;
		Input* input;

		GraphManager* graphManager;
		
		MenuGUI* menuGUI;
		EquationGUI* equationGUI;

		EquationList* equationList;
		VariableList* variableList;

		Calculator* calculator;

		TextManager* textManager;

		bool* keepWindowActive;
		bool* keepLoadingWindowActive;
		std::vector<char*> names;

		float mySliderValue;

		bool shouldTerminate = false;

		GLuint vertexShader, fragmentShader;

		Shader* shader;

};


#endif