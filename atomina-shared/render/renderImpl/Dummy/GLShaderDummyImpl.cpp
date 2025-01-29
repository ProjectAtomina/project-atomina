#include "pch.hpp"
#include "GLShaderDummyImpl.hpp"

namespace ATMA
{
    using namespace std::string_literals;

    GLShaderDummyImpl::GLShaderDummyImpl(const Shader &l_shader): GLShader(l_shader)
    {
        m_bindID = 0u;
    }

    GLShaderDummyImpl::~GLShaderDummyImpl() {}

    void GLShaderDummyImpl::compile(const ShaderType &l_type) {}
}