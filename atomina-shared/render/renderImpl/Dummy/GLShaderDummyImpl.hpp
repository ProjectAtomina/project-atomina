#pragma once
#include "pch.hpp"
#include "core/api.hpp"
#include "render/GLShader.hpp"

namespace ATMA
{

    /**
     * @brief Dummy implementation of Shader
     */
    class GLShaderDummyImpl: public GLShader
    {
    public:
        // constructor with name and filename of resource
        GLShaderDummyImpl(const Shader &l_shader);

        // default deconstructor
        virtual ~GLShaderDummyImpl();

        /**
         * @brief compiles the source code in the specified filename
         * @param l_type expected type of shader to be compiled
         */
        virtual void compile(const ShaderType &l_type) override;
    protected:
        std::string m_shaderText;
    };

}