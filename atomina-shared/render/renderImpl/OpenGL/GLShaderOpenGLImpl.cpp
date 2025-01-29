#include "pch.hpp"
#include "GLShaderOpenGLImpl.hpp"

namespace ATMA
{
    using namespace std::string_literals;

    GLShaderOpenGLImpl::GLShaderOpenGLImpl(const Shader &l_shader): GLShader(l_shader)
    {
        m_bindID = 0u;
    }

    GLShaderOpenGLImpl::~GLShaderOpenGLImpl()
    {
        if(m_compiled)
            glDeleteShader(m_bindID);
    }

    void GLShaderOpenGLImpl::compile(const ShaderType &l_type)
    {
        switch(l_type)
        {
        case ShaderType::Vertex:
            m_bindID = glCreateShader(GL_VERTEX_SHADER);
            break;
        case ShaderType::Fragment:
            m_bindID = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case ShaderType::None:
            return;
        }

        const char *l_shaderText = m_shader.m_source.c_str();
        glShaderSource(m_bindID, 1, &l_shaderText, NULL);
        glCompileShader(m_bindID);
        int success;
        char infoLog[1024];
        glGetShaderiv(m_bindID, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(m_bindID, 1024, NULL, infoLog);
            ATMA_ENGINE_ERROR("Error with compiling shader: {0} [OPENGL]: {1}", m_shader.m_name, infoLog);
        }
        m_compiled = true;
    }
}
