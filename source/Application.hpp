#pragma once

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>

class Application
{
public:
    Application();
    void Run();
    ~Application();
private:
    virtual void Start() = 0;
    virtual void Update(float dTime) = 0;
    virtual void End() = 0;
    GLFWwindow *window = nullptr;
    Application(const Application &app);
    float currentTime, lastTime, dTime;
    void main_loop();
};