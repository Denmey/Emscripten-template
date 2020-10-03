#pragma once
#include <glad/glad.h>
#include <string>
#include <optional>

class Shader
{
public:
	Shader(const std::string &vPath, const std::string &fPath);
	// Shader(const std::string& vPath, const std::string& fPath, const std::string& gPath);
	void use() const;
	GLuint getId() const;

private:
	Shader(const Shader &shader) = delete;
	static std::optional<std::string> loadFromFile(const std::string &path);
	std::string vPath, fPath, gPath;
	std::string vSource, fSource, gSource;
	GLuint id;
};