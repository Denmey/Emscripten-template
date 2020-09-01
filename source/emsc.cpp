// Based on https://gist.github.com/ousttrue/0f3a11d5d28e365b129fe08f18f4e141

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#include <emscripten/html5.h>
	#define GL_GLEXT_PROTOTYPES
	#define EGL_EGLEXT_PROTOTYPES

	EM_JS(int, window_get_inner_size,(int* w, int* h), {
		console.log(w);
		HEAP32[w>>2] = window.innerWidth;
		HEAP32[h>>2] = window.innerHeight;
	});
	EM_JS(void, screen_get_avail_size, (int* w, int* h), {
		HEAP32[w>>2] = window.screen.availWidth;
		HEAP32[h>>2] = window.screen.availHeight;
	});
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <algorithm>
#include <functional>
#include <iostream>

std::function<void()> loop;
void main_loop() { loop(); }

struct Vertex {
	glm::vec2 Pos;
	glm::vec3 Color;
	// glm::vec2 Tex;
};

std::array<Vertex, 3> vertices = {
	Vertex{{0.0f, -0.25f}, {1.0f, 0.0f, 0.0f}},
	Vertex{{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	Vertex{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
};

const char *vertexShaderSource = 
#ifdef __EMSCRIPTEN__
	"#version 300 es\n"
	"precision mediump float;\n"
#else
	"#version 330 core\n"
#endif
    "layout (location = 0) in vec2 vPos;\n"
    "layout (location = 1) in vec3 vColor;\n"

	"out vec3 fColor;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = vec4(vPos.x, vPos.y, 0.0f, 1.0f);\n"
	"   fColor = vColor;\n"
    "}\0";
const char *fragmentShaderSource = 
#ifdef __EMSCRIPTEN__
	"#version 300 es\n"
	"precision mediump float;\n"
#else
	"#version 330 core\n"
#endif
	"in vec3 fColor;\n"
    
	"out vec4 FragColor;\n"
    
	"void main()\n"
    "{\n"
    "   FragColor = vec4(fColor, 1.0f);\n"
    "}\n\0";

int main(int argc, char* argv[])
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

	GLFWwindow *window = glfwCreateWindow(800, 600, "Basic app", nullptr, nullptr);
	if (window == NULL) {
		std::cerr << "Failed to create window\n";
		return 1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return 1;
	}

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	for (int i = 0; i < count; ++i) {
		GLFWmonitor* monitor = monitors[i];
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
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
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

	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	glClearColor(1.0, 0.87, 0.83, 1.0);

    glUniform1i(0, 0);
		
	GLuint VAO = 0, VBO = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
    
	glBindVertexArray(0);

	float lastTime = glfwGetTime();
	float currentTime = glfwGetTime();
	loop = [&] {
		currentTime = glfwGetTime();
		float dt = lastTime - currentTime;
		lastTime = currentTime;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		float dAngle = 1.0;
		std::for_each(vertices.begin(), vertices.end(), [currentTime, idx = 0](Vertex& v) mutable {
			constexpr float PI_CONST = glm::pi<float>();
			v.Pos = {glm::cos(2*PI_CONST/3*idx + currentTime/10), glm::sin(2*PI_CONST/3*idx + currentTime/10)};
			++idx;
		});

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(window);
	};


#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (!glfwWindowShouldClose(window)) {
		main_loop();
	}
#endif


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
	glfwTerminate();
}
