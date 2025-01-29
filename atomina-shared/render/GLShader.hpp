#pragma once
#include "pch.hpp"
#include "core/api.hpp"
#include "util/Log.hpp"
#include "util/Path.hpp"
#include "resource/Resource.hpp"
#include "resource/ResourceEnum.hpp"

namespace ATMA
{
    /**
    * enumeration of all shader types
    */
    enum class ShaderType
    {
        None = 0,
        Vertex,
        Fragment
    };


    /**
    * Resource sub-class for shader to allow the GLShader to managed
    * by the context
    */
    class Shader: public Resource
    {
    public:
        //constructor specifing name, path, and source code
        Shader(const std::string &l_name, const Path &l_path, const std::string &l_shaderSource):
            Resource(l_name, l_path, ResType(ResourceEnum::Shader)),
            m_source(l_shaderSource)
        {
        }

        //default constructor
        Shader(): Resource("", Path{""}, ResType(ResourceEnum::Shader)), m_source("") {}
        const std::string m_source;
    };

    /**
     * @brief Reference to Shader code that can be executed on GPU
     */
    class GLShader: public LoadedResource
    {
    public:
        // default deconstructor
        virtual ~GLShader();

        /**
         * @brief compiles the shader source using GL context
         * @param l_type expected type of the shader being compiled
         */
        virtual void compile(const ShaderType &l_type) = 0;

        /**
         * @brief gets the reference id of the shader from the GL context
         * @return id of the shader
         */
        const unsigned int &getBindID() const;

        /**
         * Factory function for creating shaders
         * @param l_filePath path to file on system
         * @returns Graphics shader for the platform
         */
        static std::shared_ptr<GLShader> makeShader(const Shader &l_shader);
    protected:
        // constructor with  filename of resource
        GLShader(const Shader &l_shader);

        const Shader m_shader;
        unsigned int m_bindID;
    };

}