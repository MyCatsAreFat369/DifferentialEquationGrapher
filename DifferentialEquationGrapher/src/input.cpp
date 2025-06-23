#include <input.h>
#include <algorithm>
#include <iostream>

/*
void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}



void Input::cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{

}



void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}



void Input::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{

}


void Input::Update()
{
	
}

*/

std::vector<Input*> Input::_instances;

Input::Input(std::vector<int> keysToMonitor) : _isEnabled(true)
{
    for (int key : keysToMonitor)
    {
        _keys[key] = false;
    }
    // Add this instance to the list of instances
    Input::_instances.push_back(this);
}

Input::~Input()
{
    // Remove this instance from the list of instances
    for (int i = 0; i < _instances.size(); i++)
    {
        if (_instances[i] == this)
        {
            _instances.erase(_instances.begin() + i);
        }
    }
    //_instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}

// Public Functions

bool Input::getKeyDown(int key)
{
    // input object not enabled
    if (!_isEnabled) return false;

    // key doesn't exist
    std::map<int, bool>::iterator it = _keys.find(key);
    if (it == _keys.end()) return false;

    // returns state of key
    return _keys[key];
}

double Input::getMouseScrollX()
{
    double tempMouseScrollX = mouseScrollX;
    mouseScrollX = 0;
    return tempMouseScrollX;
}

double Input::getMouseScrollY()
{
    double tempMouseScrollY = mouseScrollY;
    mouseScrollY = 0;
    return tempMouseScrollY;
}


// Internal Functions

void Input::setKeyDown(int key, bool isDown)
{
    // iterates through all keys to check if key exists
    std::map<int, bool>::iterator it = _keys.find(key);
    if (it != _keys.end())
    {
        _keys[key] = isDown;
    }
}






// Static Functions




// Setup

void Input::setupInputs(GLFWwindow* window)
{
    glfwSetKeyCallback(window, Input::keyCallback);
    glfwSetCursorPosCallback(window, Input::cursorPositionCallback);
    glfwSetMouseButtonCallback(window, Input::mouseButtonCallback);
    glfwSetScrollCallback(window, Input::scrollCallback);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
}


// Callback Functions

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //std::cout << key << " : " << GLFW_KEY_LEFT_SHIFT << std::endl;
    // Send key event to all KeyInput instances
    for (Input* inputInstance : _instances)
    {
        inputInstance->setKeyDown(key, action != GLFW_RELEASE);
    }
}

void Input::cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    for (Input* inputInstance : _instances)
    {
        inputInstance->mouseX = xPos;
        inputInstance->mouseY = yPos;
    }
}


void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    for (Input* inputInstance : _instances)
    {
        switch (button)
        {
            case 0:
                inputInstance->mouseLeft = action != GLFW_RELEASE;
                break;
            case 1:
                inputInstance->mouseRight = action != GLFW_RELEASE;
                break;
            case 2:
                inputInstance->mouseMiddle = action != GLFW_RELEASE;
                break;
        }
    }
}


void Input::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    for (Input* inputInstance : _instances)
    {
        inputInstance->mouseScrollX = xOffset;
        inputInstance->mouseScrollY = yOffset;
    }
}