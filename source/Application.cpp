#include "Application.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <functional> // For std::bind

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

EM_JS(int, window_get_inner_size, (int *w, int *h), {
	console.log(w);
	HEAP32[w >> 2] = window.innerWidth;
	HEAP32[h >> 2] = window.innerHeight;
});
EM_JS(void, screen_get_avail_size, (int *w, int *h), {
	HEAP32[w >> 2] = window.screen.availWidth;
	HEAP32[h >> 2] = window.screen.availHeight;
});
#endif

#include <iostream>

std::function<void()> f;

void loop() {
	f();
}

void print_info();

Application::Application()
{
	std::cout << "Starting application\n";

	glfwInit();

#ifdef __EMSCRIPTEN__
	// WebAssembly
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
	// No OGL ES support here? Windows/Linux
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

	glfwWindowHint(GLFW_RESIZABLE, true);

	window = glfwCreateWindow(800, 600, "Basic app", nullptr, nullptr);
	if (window == NULL)
	{
		// std::cerr << "Failed to create window\n";
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
	{
		// std::cerr << "Failed to initialize GLAD\n";
		throw std::runtime_error("Failed to initialize GLAD");
	}

	print_info();

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

void Application::Run() {
	Start();

	f = std::bind(&Application::main_loop, this);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(loop, 0, true);
#else
	while (!glfwWindowShouldClose(window))
	{
		main_loop();
	}
#endif

	End();
}

Application::~Application()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void Application::main_loop()
{
	std::cout << "In main loop\n";
	currentTime = glfwGetTime();
	dTime = lastTime - currentTime;
	lastTime = currentTime;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	Update(dTime);
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void print_info() {
	
	int count;
	GLFWmonitor **monitors = glfwGetMonitors(&count);

	for (int i = 0; i < count; ++i)
	{
		GLFWmonitor *monitor = monitors[i];
		std::cout << "Monitor #" << i << (i == 0 ? "(Primary)" : "") << ":\n";

#ifdef __EMSCRIPTEN__
		int w, h, isF;
		std::cout << &w << "\n";
		emscripten_get_canvas_size(&w, &h, &isF);
		std::cout << "   Canvas size: (" << w << ", " << h << ");\n";
		double dw, dh;
		emscripten_get_element_css_size("canvas", &dw, &dh);
		std::cout << "   Canvas CSS size: (" << dw << ", " << dh << ");\n";
		emscripten_get_element_css_size("body", &dw, &dh);
		std::cout << "   Page CSS size: (" << dw << ", " << dh << ");\n";
		window_get_inner_size(&w, &h);
		std::cout << "   Window inner size: (" << w << ", " << h << ");\n";
		screen_get_avail_size(&w, &h);
		std::cout << "   Screen available size: (" << w << ", " << h << ");\n";
#endif
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
		std::cout << "   Resolution: (" << vidmode->width << ", " << vidmode->height << ");\n";
		std::cout << "   Number of bits for each channel: ("
		          << vidmode->redBits << ", "
		          << vidmode->greenBits << ", "
		          << vidmode->blueBits << ");\n";
		std::cout << "   Refresh rate: " << vidmode->refreshRate << "\n";
	}
	std::cout << "OpenGL Info:\n";
	std::cout << "   Vendor: " << glGetString(GL_VENDOR) << "\n";
	std::cout << "   Renderer: " << glGetString(GL_RENDERER) << "\n";
	std::cout << "   GL version: " << glGetString(GL_VERSION) << "\n";
	std::cout << "   GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}
