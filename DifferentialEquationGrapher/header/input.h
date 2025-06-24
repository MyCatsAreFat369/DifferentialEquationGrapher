#ifndef INPUT_CLASS_H
#define INPUT_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>
#include <vector>

class Input {
    // Main Input functionality
    public:
        // Takes a list of which keys to keep state for
        Input();
        Input(std::vector<int> keysToMonitor);
        ~Input();

        // If this Input is enabled and the given key is monitored,
        // returns pressed state.  Else returns false.
        bool getKeyDown(int key);

        double getMouseX() { return mouseX; }
        double getMouseY() { return mouseY; }

        bool leftMouseButtonDown() { return mouseLeft; }
        bool rightMouseButtonDown() { return mouseRight; }
        bool middleMouseButtonDown() { return mouseMiddle; }

        double getMouseScrollX();
        double getMouseScrollY();

        bool isEnabled() { return _isEnabled; }
        void Enable(bool value) { _isEnabled = value; }
    private:
        // Used internally to update key states.  Called by the GLFW callback.
        void setKeyDown(int key, bool isDown);
        std::map<int, bool> _keys;
        bool mouseLeft = false, mouseRight = false, mouseMiddle = false;
        double mouseX = 0.0, mouseY = 0.0;
        double mouseScrollX = 0.0, mouseScrollY = 0.0;

        bool _isEnabled;

        // Workaround for C++ class using a c-style-callback
    public:
        // Must be called before any Input instances will work
        static void setupInputs(GLFWwindow* window);
    private:
        // The GLFW callback for input events.  Sends events to all Input instances
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
        // Keep a list of all Input instances and notify them all of key events
        static std::vector<Input*> _instances;
};

#endif