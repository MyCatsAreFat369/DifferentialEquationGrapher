#ifndef EQUATION_GUI_CLASS_H
#define EQUATION_GUI_CLASS_H


#include "calculator/equationList.h"
#include "grapher/graphManager.h"

class EquationGUI
{
	public:
		EquationGUI(EquationList* equationList, GraphManager* graphManager);
		void construct(int width, int height);

		bool isHoveringImGui = false;
	private:
		// add some members
		EquationList* equationList;
		GraphManager* graphManager;

		bool* keepWindowActive;

		bool construct_equation_element(int id);
};


#endif