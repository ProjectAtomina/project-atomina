#pragma once
#include "pch.hpp"
#include "core/api.hpp"
#include "Log.hpp"

namespace ATConst
{

    static const sf::Image &getDefaultImage()
    {
        ATMA_ENGINE_INFO("Default Image is being used");
        static sf::Image img;
        static const sf::Uint8 pixels[16]{
            0xFF,
            0x00,
            0xFF,
            0xFF,
            0x00,
            0x00,
            0x00,
            0xFF,
            0x00,
            0x00,
            0x00,
            0xFF,
            0xFF,
            0x00,
            0xFF,
            0xFF};
        img.create({2u, 2u}, pixels);
        return img;
    }

    static const size_t OBJECT_BIT_SIZE = 32;

}
