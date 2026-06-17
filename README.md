## A bit on this project

Meant to be an educational tool and not a professional workspace.
The latter is the aim for the next release with JavaFX, but still mainly focusing on educational quality.

Highly inspired by Desmos UI, namely:
- The layout of variables
- The variable manipulation (eg. min, max, step)
- The coloring of curves
- The grid itself

This differential equation grapher allows the user to:
- Graph ODEs (Ordinary Differential Equations) of any degree
- Granted that the ODE is of the form y'' = f(t, y, y') for example
- May be used to graph normal equations as well (to verify if a solution is correct)

Some limitations include:
- Not being able to graph vertical asymptotes correctly
- Limited manipulation of initial conditions; they can't be assigned to variables or be given as an expression of variables
- Not being able to graph other types of differential equations, of course
- Using Forward-Euler to solve all equations, which imposes limitations on the precision of the solutions
- Can't solve boundary condition problems

### Running the software

You may go to the Releases section and download the latest release (there will probably be only one for now). Unzip the file and run the executable. Windows will probably warn you but that's because I haven't registered this software with Windows, probably won't do for the C++ version. You should get a screen that looks similar to the one below.

## How to use the grapher to solve ODEs

Upon entering the grapher, you should see this view:
<img width="1919" height="1023" alt="image" src="https://github.com/user-attachments/assets/929ec299-0b06-4f4c-8eba-6215c9ffe474" />

It may look daunting at first, but take note of the **Equations** section:

<img width="392" height="475" alt="image" src="https://github.com/user-attachments/assets/603c9cfb-a111-49e5-a655-6c19c0df2889" />

You may type a differential equation using proper syntax (as will be described in detail below) and the grapher will attempt to solve the differential equation using **Forward-Euler**.
If you select **First-Order**, it will solve equations of the form y' = f(t, y), whereas **Second-Order** would solve equations of the form y'' = f(t, y, y'). Of course, **Multi-Order** lets you graph higher-order ODEs,
as seen with the slider:

<img width="386" height="428" alt="image" src="https://github.com/user-attachments/assets/1fddd1f7-35a3-4ff2-b35d-348400d71381" />

To graph the differential equation, simply click **Compile**.
To hide extra options (initial conditions, color) and show only the equation itself, click **Collapse**. You may click **Expand** to show it again after.
You may also hide the specific curve by unchecking **Draw Curve**.

By manipulating initial conditions, we may seek different solutions to the initial-value problem:

<img width="800" height="429" alt="2026-06-1515-32-55-ezgif com-crop" src="https://github.com/user-attachments/assets/8ead3480-7a83-4401-9174-ffd14fac1409" />

Furthermore, you are given the ability to graph normal equations. This may be useful for confirming if your computed solution is valid:

<img width="1919" height="1020" alt="image" src="https://github.com/user-attachments/assets/5e3ea207-980e-4af9-b931-cda3095b33ef" />

You may make use of variable definitions for more complicated differential equations:

<img width="1919" height="1027" alt="image" src="https://github.com/user-attachments/assets/ad4e9cf4-673a-4fb4-a599-77de3bfb802b" />

The above example defines variables "g", "b" and "m" to solve the classic problem of free fall with air friction.

Any type of variable, be it initial condition or user-defined variable, has the following options:
- **Type in** - allows the user to type in a value for the variable
- **Done** - appears in place of "**Type in**" when in typing mode. Used to switch back to slider mode
- **Min** - Allows for setting a minimum value for the variable. The variable may not go below this value
- **Max** - Allows for setting a maximum value for the variable. The variable may not exceed this value
- **Step** - Imposes a step function on the variable, namely if the variable has a step of 0.1 it may only take values 0.0, 0.1, 0.2, 0.3 etc.

You may **save** your project by going to File->Save or Save As, and entering your filename.
<img width="421" height="185" alt="image" src="https://github.com/user-attachments/assets/abc9417d-30c0-4d6f-9dda-32c870751aa0" />
<img width="356" height="410" alt="image" src="https://github.com/user-attachments/assets/a629c9e8-0651-40e0-8c36-774f14226ba7" />

### Syntax

This section is devoted to describing the syntax of the grapher.

First of all, the grapher generally follows PEMDAS. It uses Postfix to wrap all expressions with parenthesis in order to then create the executable list of tokens which are executed by following the rules of
postfix.

You may do addition by adding two elements as follows:
- 3 + a

Or subtraction:
- 3 - a

However, this isn't supported (FOR VARIABLES ONLY):
- -a + 3

The compiler gets confused. This is a limitation I never fixed. You may instead do:
- 0 - a + 3

For multiplication, you may do the following:
- 3 * a
- 3a
- 3(f + 1)
- f(f + 1)
- g(t)(t + 1) -# Meaning g(t) times (t + 1) for a normal function g(t)
- (t + 1)(t + 2)

For division, you may do:
- 3 / a
- 3 * a / b -# And it will be the same as doing (3 * a) / b
- 2 * 3 / 4 * 5 -# Note: this will translate to (2 * 3) / (4 * 5) due to the fact that multiplication is favored over division in PEMDAS

For exponentiation you may do:
- e^x
- 2^(a + b)
- 2^a * 3^b -# And it will be the same as doing (2^a) * (3^b)
- 2^2^2 -# And it will be the same as doing (2^2)^2, to avoid this do 2^(2^2) explicitly

Any use of extra parentheses is allowed if desired.

There are a few designated function names, designed to make your life easier. These are the following:
- sin
- asin
- cos
- acos
- tan
- atan
- sinh
- asinh
- cosh
- acosh
- tanh
- atanh
- ln

To use any sort of function, be it designated or user-defined, you may simply use the function name followed by parenthesis. The following syntax works:
- sin(t)
- ln(1 + f(t))
- sin(t) * cos(t)

However, you may not do:
- sin(t)cos(t)

All ODEs in this grapher use the variable **t** to describe the independent variable of the ODE. This is reminiscient of the "time" variable in physics equations, but is just flavour syntax. Unfortunately
the grapher doesn't give you the ability to change the name of the independent variable.

There are a few rules as to designated syntax:
- All function/variable names may be 1 to 4 characters in total. They may contain uppercase/lowercase latin characters, and underscores _
- Function/variable names may not conflict with designated function names such as sin(x) or constants like e and π
- A differential equation's (eg. y' = -y) function/solution (eg. y) may be used in other differential equations to form a system of differential equations. Its derivatives may be referenced by using y' or y1, y'' or y2 etc.
- A differential equation's function/solution may not be inserted into a normal function's definition, therefore disallowing the manipulation of solutions to differential equations.
- A normal function may be inserted into a differential equation call.
- You may not define normal functions in the following way: f as a function of t and g, and g as a function of t and f. This is a circular dependency and the grapher cannot resolve it.
- You may not access a function's highest derivatives, for example if we have y'' = -y then you may not access y'' or y3 and higher.
- You may graph only a function's n-2 derivatives by the following rule: If y_n = f(t, y, y1, ..., y_n-1) then you may only graph y_n-2, y_n-3, ... , y1, y. For example, y'' = y only allows y to be graphed. You may do so by forming a system of ODEs which follows y = f(t, y,...) and g = y1 to graph y or g = y to graph its integral

### Thank you for reading until the end!

Made by maple
