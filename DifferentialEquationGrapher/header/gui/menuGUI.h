#ifndef MENU_GUI_CLASS_H
#define MENU_GUI_CLASS_H

#include "calculator/calculatorDefs.h"

class MenuGUI
{
	public:
		MenuGUI(EquationList* equationList, VariableList* variableList);
		void construct(int width, int height);
	private:
		EquationList* equationList;
		VariableList* variableList;

		bool* keepWindowActive;

		bool doStuff = false;
};



#endif