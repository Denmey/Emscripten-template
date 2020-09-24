// Based on https://gist.github.com/ousttrue/0f3a11d5d28e365b129fe08f18f4e141

#include "Application.hpp"

#include "imgui.h"

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <algorithm>
#include <iostream>


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

struct Vertex
{
	glm::vec2 Pos;
	glm::vec3 Color;
	// glm::vec2 Tex;
};


class App : public Application {

	std::array<Vertex, 3> vertices = {
		Vertex{{0.0f, -0.25f}, {1.0f, 0.0f, 0.0f}},
		Vertex{{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		Vertex{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	};
	GLuint VAO = 0, VBO = 0;
	GLuint shaderProgram = 0;
	float scale = 1.0f;


	void Start() override {
		glClearColor(1.0, 0.87, 0.83, 1.0);
		// glUniform1i(0, 0);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);

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
			std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
			          << infoLog << std::endl;
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
			std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
			          << infoLog << std::endl;
		}
		// link shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		// check for linking errors
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
			          << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void Update(float dTime) override {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (ImGui::Begin("Window")) {
			ImGui::SliderFloat("Scale", &scale, 0.1f, 5.0f);
		}
		ImGui::End();

		ImGui::ShowDemoWindow();

		float time = glfwGetTime();
		float scale = this->scale;
		std::for_each(vertices.begin(), vertices.end(), [time, scale, idx = 0](Vertex &v) mutable {
			constexpr float PI_CONST = glm::pi<float>();
			v.Pos = {glm::cos(2 * PI_CONST / 3 * idx + time / 10), glm::sin(2 * PI_CONST / 3 * idx + time / 10)};
			v.Pos *= scale/5.0f;
			++idx;
		});

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void End() override {

	}
};

int main(int argc, char *argv[])
{
	App app;
	app.Run();
}
