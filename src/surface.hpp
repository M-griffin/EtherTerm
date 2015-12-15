#ifndef SURFACE_HPP
#define SURFACE_HPP

#ifdef TARGET_OS_MAC
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <iostream>
#include <string>

#include <boost/smart_ptr/shared_ptr.hpp>

/**
 * @class Surfaces
 * @author Michael Griffin
 * @date 12/11/2015
 * @file surface_manager.hpp
 * @brief Dynamic Container to Hold Surfaces
 */
class Surfaces
{

public:
    Surfaces(SDL_Surface *surface)
        : m_surface(surface)
        , m_is_converted(false)
    {
    }

    ~Surfaces()
    {
        std::cout << "surfaces: Surface Removed!" << std::endl;
        SDL_FreeSurface(m_surface);
        m_surface = nullptr;
    }

    SDL_Surface *getSurface()
    {
        if (!m_surface)
        {
            SDL_Log("Surface is null!");
        }
        return m_surface;
    }

    /**
     * @brief Convert The Surface to match Texutre Pixel Format
     * @return
     */
    bool convert()
    {
        if (!m_is_converted)
        {
            m_surface = SDL_ConvertSurfaceFormat(m_surface, SDL_PIXELFORMAT_ARGB8888, 0);
            if(!m_surface)
            {
                SDL_Log("Unable to Convert Surface");
                return false;
            }
            m_is_converted = true;
        }
        return true;
    }

    /**
     * @brief Clear the Surface with all Black.
     * @return
     */
    void clear()
    {
        SDL_FillRect(m_surface, nullptr, SDL_MapRGBA(m_surface->format, 0, 0, 0, 0));
    }

    /**
     * @brief Make sure Surface is not null!
     * @return
     */
    bool exists()
    {
        return (m_surface != nullptr);
    }

private:
    SDL_Surface *m_surface;
    bool         m_is_converted;

};

typedef boost::shared_ptr<Surfaces> surface_ptr;


#endif // SURFACE_HPP
