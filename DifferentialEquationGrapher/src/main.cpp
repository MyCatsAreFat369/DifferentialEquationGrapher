#include "application.h"

int main()
{
	Application* application = new Application();
	
	application->loop();

	application->Delete();
	delete application;

	return 0;
}