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
            unsigned short formatChunkSize{};
            unsigned char format{};
            unsigned char channels{};
            unsigned short sampleRate{};
            unsigned char sampleSize{};
            unsigned short dataSize{};
            std::copy(
                buffer.begin() + 16,
                buffer.begin() + 16 + sizeof(unsigned short),
                reinterpret_cast<unsigned char *>(&formatChunkSize)
            );
            std::copy(
                buffer.begin() + 20,
                buffer.begin() + 20 + sizeof(unsigned char),
                reinterpret_cast<unsigned char *>(&format)
            );
            std::copy(
                buffer.begin() + 22,
                buffer.begin() + 22 + sizeof(unsigned char),
                reinterpret_cast<unsigned char *>(&channels)
            );
            std::copy(
                buffer.begin() + 24,
                buffer.begin() + 24 + sizeof(unsigned short),
                reinterpret_cast<unsigned char *>(&sampleRate)
            );
            std::copy(
                buffer.begin() + 34,
                buffer.begin() + 34 + sizeof(unsigned char),
                reinterpret_cast<unsigned char *>(&sampleSize)
            );
            std::copy(
                buffer.begin() + 40,
                buffer.begin() + 40 + sizeof(unsigned short),
                reinterpret_cast<unsigned char *>(&dataSize)
            );
            std::vector<unsigned int> data{};
            data.reserve(dataSize);
            for(int i = 1; i <= dataSize; i++)
            {
                if(i%8 == 0)
                {
                    ATMA_ENGINE_TRACE("Copying {} bytes to audio buffer", (i - (i-8)));
                    unsigned int slice;
                    std::copy(
                        buffer.begin() + 40 + i - 8,
                        buffer.begin() + 40 + i,
                        reinterpret_cast<unsigned char *>(&slice)
                    );
                    data.emplace_back(slice);
                }
                
            }
            ATMA_ENGINE_INFO("loading .wav formatChunkSize={} format={} channels={} sampleRate={} sampleSize={} dataSize={}", formatChunkSize, format, channels, sampleRate, sampleSize, dataSize);
            Wave w{l_name, l_path, channels, sampleRate, sampleSize, data.data(), dataSize};
            return std::make_shared<AudioWave>(w);
        }
        return nullptr;
    }

    std::shared_ptr<AudioWave> ResourceLoader<AudioWave>::load(const std::string &l_name)
    {
        Wave w{l_name, ATMA::Path{""}, 0, 0, 0, nullptr, 0};
        return std::make_shared<AudioWave>(w);
    }


}