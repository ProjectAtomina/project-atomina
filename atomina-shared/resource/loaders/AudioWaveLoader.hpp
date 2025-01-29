#pragma once
#include "pch.hpp"
#include "core/api.hpp"
#include "resource/ResourceLoader.hpp"
#include "util/Log.hpp"
#include "sound/AudioWave.hpp"

namespace ATMA
{
    template<>
    class ResourceLoader<AudioWave>
    {
    public:
        ResourceLoader();
        virtual ~ResourceLoader();
        std::shared_ptr<AudioWave> load(const std::string &l_name, const Path &l_path);
        std::shared_ptr<AudioWave> load(const std::string &l_name);
    };
}