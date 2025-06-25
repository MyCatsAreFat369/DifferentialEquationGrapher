#ifndef TOOLTIPS_CLASS_H
#define TOOLTIPS_CLASS_H

const char* equationTooltip = "Please enter a valid differential equation, then click \"Compile\" to graph it.\n\n"
				  "To graph a first-order differential equation, you should input the function's name, and then use it "
				  "in the equation accordingly. For example, when you want to graph y' = Ay + 1, you should simply "
				  "input Ay + 1. You can use any name for your function, but I'll be using y as an example here.\n\n"

				  "For a second-order differential equation, you should use y and y' as the functions. For example, "
				  "to graph y'' = Ay' + By + Cyy', simply input Ay' + By + Cyy'.\n\n"

				  "Generally, any multi-order differential equation should have anything but its lowest derivative in the equation. "
				  "To express high-order derivatives, instead of doing y'''', you could do y4 for instance. The limit to how much you "
				  "can differentiate a function is 9, so you can do y9 but not y10.\n\n"

				  "In this graphing calculator, y is a function of t, hence y(t). You should only include t as a variable in the "
				  "differential equation. Inputting x will simply make a new constant variable. As a result, you can't use "
				  "t as a function or constant variable name.\n\n"

				  "Any name used as a function name can't be used for a constant variable's name, and vice versa.\n\n"

				  "You could input something like this for your equation:\n\n"
				  "Ay3 + By'' - 10t + kA";

const char* variableTooltip = "Please enter a valid variable, then click \"Update Variable Name\" to update it.\n\n"

				  "In this software, variable and function names have a set maximum length of 4 characters. Furthermore, "
				  "there's a limitation on which characters you could use to name a function or variable.\n\n"

				  "You can't name a constant variable after a function or its derivatives. Additionally, "
				  "numbers and special characters excluding _ are not allowed in variable and function names.\n\n"

				  "For instance, you could create the following variables:\n\n"

				  "kA\n"
				  "b\n"
				  "GLFW\n"
				  "A_k\n\n"

				  "But not the following:\n\n"

				  "My_variable\n"
				  "k1_a\n"
				  "!A\n"
				  "f (when there's a function named f)\n"
				  "f3 or f'\n";



#endif