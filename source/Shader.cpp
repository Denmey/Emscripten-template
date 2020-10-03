#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <optional>

constexpr auto coreVersionLine = "#version 330 core\n";
constexpr auto esVersionLine   = "#version 300 es\n";
constexpr auto precisionLine   = "precision mediump float;\n";

struct StringHelper {
	const char *p;
	StringHelper(const std::string& s) : p(s.c_str()) {}
	operator const char**() { return &p; }
};

static int compileShader(int shaderType, const std::string& source) {
	int shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, StringHelper(source), NULL);
	glCompileShader(shaderId);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cerr << "   Error: shader compilation is failed.\n"
		          << infoLog << std::endl;
	}
	return shaderId;
}

Shader::Shader(const std::string &vPath, const std::string &fPath)
{
	this->vPath = vPath;
	this->fPath = fPath;

	if (auto opt = Shader::loadFromFile(vPath))
	{
		this->vSource = *opt;
	}
	if (auto opt = Shader::loadFromFile(fPath))
	{
		this->fSource = *opt;
	}

	id = glCreateProgram();
	auto vertexShader   = compileShader(GL_VERTEX_SHADER, vSource);
	auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, fSource);
	// link shaders
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);
	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cerr << "   Error: shader linking is failed:\n"
		          << infoLog << std::endl;
		id = 0;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// std::cout << "   Shader was successfully loaded.\n";
}

GLuint Shader::getId() const
{
	return id;
}

void Shader::use() const
{
	glUseProgram(id);
}

std::optional<std::string> Shader::loadFromFile(const std::string &path)
{
	std::cout << "Loading shader from '" + path + "':\n";
	std::ifstream file;
	file.open(path);
	if (file.is_open()) {
		std::string original{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
		const auto versionBegin    = original.find("#version");
		const auto versionEnd      = original.find('\n', versionBegin);
		const auto versionLen      = versionEnd-versionBegin+1;
		// TODO: Remove substr
		const auto isCore          = original.substr(versionBegin, versionLen).find("core") != std::string::npos;
		const auto isEs            = original.substr(versionBegin, versionLen).find("es") != std::string::npos;
		const auto hasPrecision    = original.find("\nprecision ") != std::string::npos;
		// std::cout << original.substr(versionBegin, versionLen);
		// std::cout << versionBegin << ":" << versionEnd << "\n";
		// std::cout << isCore << ":" << isEs << ":" << hasPrecision << ":" << std::string::npos << "\n";
		auto result = original;
#ifdef __EMSCRIPTEN__
		if (isCore) {
			std::cout << "   Warning: replacing core version with 300 es.\n";
			result.replace(versionBegin, versionEnd, esVersionLine);
		}
		if (!hasPrecision) {
			std::cout << "   Warning: adding mediump precision.\n";
			result.insert(versionEnd, precisionLine);
		}
#else
		if (isEs) {
			std::cout << "   Warning: replacing es version with 330 core.\n";
			result.replace(versionBegin, versionEnd, coreVersionLine);
		}
#endif
		file.close();
		// std::cout << result << "\n";

		return result;
	} else {
		std::cout << "   Error: Couldn't load shader from '" + path + "': check if file exists.\n";
		return std::nullopt;
	}
	return std::nullopt;
}