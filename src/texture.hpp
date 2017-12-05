#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <iostream>
#include <string>
#include <memory>

/**
 * @class Textures
 * @author Michael Griffin
 * @date 12/11/2015
 * @file surface_manager.hpp
 * @brief Dynamic Container to Hold Textures
 */
class Textures
{

public:
    Textures(SDL_Texture *texture)
        : m_texture(texture)
    {
        std::cout << "A Texture was Created!" << std::endl;
    }

    ~Textures()
    {
        std::cout << "~A Texture was removed!" << std::endl;
        if (m_texture)
            SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }

    SDL_Texture *getTexture()
    {
        if (!m_texture)
        {
            SDL_Log("Texture is null!");
        }
        return m_texture;
    }

    /**
     * @brief Make sure Surface is not null!
     * @return
     */
    bool exists()
    {
        return (m_texture != nullptr);
    }

private:
    SDL_Texture *m_texture;

};

typedef std::shared_ptr<Textures> texture_ptr;
#endif // TEXTURE_HPP
