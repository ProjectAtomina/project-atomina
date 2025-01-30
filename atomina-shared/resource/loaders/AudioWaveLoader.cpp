#include "pch.hpp"
#include "AudioWaveLoader.hpp"

namespace ATMA
{
	ResourceLoader<AudioWave>::ResourceLoader()
    {

    }

    ResourceLoader<AudioWave>::~ResourceLoader()
    {

    }

    std::shared_ptr<AudioWave> ResourceLoader<AudioWave>::load(const std::string &l_name, const Path &l_path)
    {
        std::basic_ifstream<unsigned char> audioFile{l_path.toString(), std::ios_base::binary};
        std::vector<unsigned char> buffer;
        audioFile.seekg(0, audioFile.end);
        size_t length = audioFile.tellg();
        audioFile.seekg(0, audioFile.beg);
        if(length > 0)
        {
            buffer.resize(length);
            audioFile.read(&buffer[0], length);
            unsigned int formatChunkSize{};
            unsigned short format{};
            unsigned short channels{};
            unsigned int sampleRate{};
            unsigned short sampleSize{};
            unsigned int dataSize{};
            std::copy(
                buffer.begin() + 16,
                buffer.begin() + 16 + sizeof(unsigned int),
                reinterpret_cast<unsigned char *>(&formatChunkSize)
            );
            std::copy(
                buffer.begin() + 20,
                buffer.begin() + 20 + sizeof(unsigned short),
                reinterpret_cast<unsigned char *>(&format)
            );
            std::copy(
                buffer.begin() + 22,
                buffer.begin() + 22 + sizeof(unsigned short),
                reinterpret_cast<unsigned char *>(&channels)
            );
            std::copy(
                buffer.begin() + 24,
                buffer.begin() + 24 + sizeof(unsigned int),
                reinterpret_cast<unsigned char *>(&sampleRate)
            );
            std::copy(
                buffer.begin() + 34,
                buffer.begin() + 34 + sizeof(unsigned short),
                reinterpret_cast<unsigned char *>(&sampleSize)
            );
            std::copy(
                buffer.begin() + 40,
                buffer.begin() + 40 + sizeof(unsigned int),
                reinterpret_cast<unsigned char *>(&dataSize)
            );
            
            std::vector<unsigned char> data{};
            data.reserve(dataSize);
            for(int i = 0; i < dataSize; i++)
            {
                data.emplace_back(buffer[44+i]);
            }
            ATMA_ENGINE_INFO("loaded .wav formatChunkSize={} format={} channels={} sampleRate={} sampleSize={} dataSize={}", formatChunkSize, format, channels, sampleRate, sampleSize, dataSize);
            Wave w{l_name, l_path, channels, sampleRate, sampleSize, data};
            return std::make_shared<AudioWave>(w);
        }
        return nullptr;
    }

    std::shared_ptr<AudioWave> ResourceLoader<AudioWave>::load(const std::string &l_name)
    {
        Wave w{l_name, ATMA::Path{""}, 0, 0, 0, {}};
        return std::make_shared<AudioWave>(w);
    }


}