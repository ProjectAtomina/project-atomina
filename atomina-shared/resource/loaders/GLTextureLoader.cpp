#include "pch.hpp"
#include "GLTextureLoader.hpp"

namespace ATMA
{

    ResourceLoader<GLTexture>::ResourceLoader() {}

    ResourceLoader<GLTexture>::~ResourceLoader() {}

    std::shared_ptr<GLTexture> ResourceLoader<GLTexture>::load(const std::string &l_name, const Path &l_path)
    {
        int width, height, channels;
        unsigned char *data = stbi_load(l_path.toString().c_str(), &width, &height, &channels, 0);
        Texture texture = Texture(l_name, l_path, width, height, channels, data);
        return GLTexture::makeTexture(texture);
    }

    std::shared_ptr<GLTexture> ResourceLoader<GLTexture>::load(const std::string &l_name)
    {
        Texture texture{l_name, ATMA::Path{""}, 0,0,0,0};
        return GLTexture::makeTexture(texture);
    }
}