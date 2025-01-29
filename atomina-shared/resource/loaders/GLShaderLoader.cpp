#include "pch.hpp"
#include "GLShaderLoader.hpp"

using namespace std::string_literals;

namespace ATMA
{

    ResourceLoader<GLShader>::ResourceLoader() {}

    ResourceLoader<GLShader>::~ResourceLoader() {}

    std::shared_ptr<GLShader> ResourceLoader<GLShader>::load(const std::string &l_name, const Path &l_path)
    {
        std::string shaderText = ""s;
        std::ifstream l_fileStream{l_path.toString()};
        std::string sourceLine;
        while(std::getline(l_fileStream, sourceLine))
            shaderText += sourceLine + "\n";
        l_fileStream.close();
        Shader shader{l_name, l_path, shaderText};
        return GLShader::makeShader(shader);
    }

    std::shared_ptr<GLShader> ResourceLoader<GLShader>::load(const std::string &l_name)
    {
        Shader shader{l_name, ATMA::Path{""}, ""};
        return GLShader::makeShader(shader);
    }
}