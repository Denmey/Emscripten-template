// Based on https://gist.github.com/ousttrue/0f3a11d5d28e365b129fe08f18f4e141

#include "Application.hpp"
#include "Shader.hpp"


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
	Shader basicShader{"resources/shaders/basic.vs", "resources/shaders/basic.fs"};
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

		basicShader.use();
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
