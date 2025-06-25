#ifndef EQUATION_GUI_CLASS_H
#define EQUATION_GUI_CLASS_H

#include "calculator/equationList.h"
#include "grapher/graphManager.h"

class EquationGUI
{
	public:
		EquationGUI(EquationList* equationList, VariableList* variableList, GraphManager* graphManager);
		void construct(int width, int height);

		bool isHoveringImGui = false;
	private:
		// add some members
		EquationList* equationList;
		VariableList* variableList;
		GraphManager* graphManager;

		bool* keepWindowActive;

		bool construct_equation_element(int id);
		bool construct_variable_element(int id);

		void CreateTooltip(const char* name, const char* desc);
};


#endif