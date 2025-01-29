#include "pch.hpp"
#include "AudioWave.hpp"

namespace ATMA
{
    AudioWave::~AudioWave() {}

    AudioWave::AudioWave(const Wave &l_wave): m_wave(l_wave) {}

}