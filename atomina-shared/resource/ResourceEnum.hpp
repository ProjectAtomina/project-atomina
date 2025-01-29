#pragma once
#include "pch.hpp"

namespace ATMA
{

    using ResourceType = unsigned int;

    /**
     * Enumeration of all possible system types
     */
    enum class ResourceEnum
    {
        None = 0u,
        Texture = 1u,
        Font,
        Shader,
        Waveform,
        COUNT
    };

    /**
     * wrapper class for easy conversion from system enum to
     * unsigned int
     */
    class ResType
    {
    public:
        // constructor with name
        ResType(const ResourceEnum &l_type): m_type(l_type) {}

        // conversion constructor
        ResType(const unsigned int &l_int): m_type(static_cast<ResourceEnum>(l_int)) {}

        // conversion operator
        operator unsigned int()
        {
            return static_cast<unsigned int>(m_type);
        }
    private:
        const ResourceEnum m_type;
    };
}