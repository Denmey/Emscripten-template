#pragma once
#include <glad/glad.h>
#include <string>
#include <optional>

class Shader
{
public:
	Shader(const std::string &vPath, const std::string &fPath);
	// Shader(const std::string& vPath, const std::string& fPath, const std::string& gPath);
	void reload();
	void setHotReload(bool value);
	void use() const;

private:
	Shader(const Shader &shader);
	static std::optional<std::string> loadFromFile(const std::string &path);
	std::string vPath, fPath, gPath;
	std::string vSource, fSource, gSource;
	GLuint id;
};