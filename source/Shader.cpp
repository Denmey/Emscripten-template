#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <optional>

constexpr auto coreVersionLine = "#version 330 core\n";
constexpr auto esVersionLine   = "#version 300 es\n";
constexpr auto precisionLine   = "precision mediump float;\n";


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

	// std::cout << "Shader was successfully loaded.\n";

}

void Shader::reload()
{
}

void Shader::setHotReload(bool value)
{
}

void Shader::use() const
{
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
		const auto versionLen      = versionEnd-versionBegin+2;
		std::cout << original << "\n";
		const auto isCore          = original.find("core",      versionBegin, versionLen);
		const auto isEs            = original.find("es",        versionBegin, versionLen);
		const auto hasPrecision    = (original.find("precision", versionBegin, versionLen));
		std::cout << original.substr(versionBegin, versionLen);
		std::cout << versionBegin << ":" << versionEnd << "\n";
		std::cout << isCore << ":" << isEs << ":" << hasPrecision << ":" << std::string::npos << "\n";
		file.close();
		return std::string{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
	} else {
		std::cout << "Error: Couldn't load shader from '" + path + "'.\n";
		return std::nullopt;
	}
	return std::nullopt;
}