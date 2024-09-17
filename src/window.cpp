#include "Window.hpp"
#include "ErrorLogger.hpp"
#include "Music.hpp"

namespace kn::window
{
static SDL_Renderer* _renderer;
static SDL_Window* _window;
static Event _event;
static std::vector<Event> _events;

void init(const math::Vec2& size, const std::string& title, int scale)
{
    if (_renderer)
        WARN("Cannot initialize renderer more than once")

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        FATAL("SDL_Init Error: " + std::string(SDL_GetError()))
    }
    if (!IMG_Init(IMG_INIT_PNG))
    {
        FATAL("IMG_Init Error: " + std::string(IMG_GetError()))
        SDL_Quit();
    }
    if (TTF_Init())
    {
        FATAL("TTF_Init Error: " + std::string(TTF_GetError()))
        IMG_Quit();
        SDL_Quit();
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048))
    {
        FATAL("Mix_OpenAudio Error: " + std::string(Mix_GetError()))
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    _window = SDL_CreateWindow("Kraken Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               (int)size.x * scale, (int)size.y * scale, SDL_WINDOW_SHOWN);

    if (!_window)
        FATAL("SDL_CreateWindow Error: " + std::string(SDL_GetError()))

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

    if (!_renderer)
        FATAL("SDL_CreateRenderer Error: " + std::string(SDL_GetError()));

    if (scale > 1)
    {
        SDL_RenderSetLogicalSize(_renderer, (int)size.x, (int)size.y);
        SDL_RenderSetIntegerScale(_renderer, SDL_TRUE);
    }

    setTitle(title);
}

void quit()
{
    if (_renderer)
        SDL_DestroyRenderer(_renderer);
    if (_window)
        SDL_DestroyWindow(_window);
    _events.clear();
    _event = Event();
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    music::unload();
}

const std::vector<Event>& getEvents()
{
    if (!_window)
        WARN("Cannot get events before creating the window")

    _events.clear();
    while (SDL_PollEvent(&_event))
        _events.push_back(_event);

    return _events;
}

void clear(Color color)
{
    if (!_renderer)
        WARN("Cannot clear screen before creating the window")

    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(_renderer);
}

void flip()
{
    if (!_renderer)
        WARN("Cannot flip screen before creating the window")

    SDL_RenderPresent(_renderer);
}

void blit(const Texture& texture, const Rect& dstRect, const Rect& srcRect)
{
    if (!_renderer)
        WARN("Cannot blit before creating the window")

    if (math::Vec2{srcRect.x, srcRect.y} == math::Vec2())
    {
        SDL_RenderCopyF(_renderer, texture.getSDLTexture(), nullptr, &dstRect);
        return;
    }

    SDL_Rect src;
    src.x = (int)srcRect.x;
    src.y = (int)srcRect.y;
    src.w = (int)srcRect.w;
    src.h = (int)srcRect.h;

    SDL_RenderCopyF(_renderer, texture.getSDLTexture(), &src, &dstRect);
}

void blit(const Texture& texture, const math::Vec2& position)
{
    if (!_renderer)
        WARN("Cannot blit before creating the window")

    Rect rect = {(float)position.x, (float)position.y, (float)texture.getSize().x,
                 (float)texture.getSize().y};

    SDL_RenderCopyF(_renderer, texture.getSDLTexture(), nullptr, &rect);
}

void blitEx(const Texture& texture, const Rect& dstRect, const Rect& srcRect, double angle,
            bool flipX, bool flipY)
{
    if (!_renderer)
        WARN("Cannot blit before creating the window")

    SDL_RendererFlip flipAxis = SDL_FLIP_NONE;
    if (flipX)
        flipAxis = (SDL_RendererFlip)(flipAxis | SDL_FLIP_HORIZONTAL);
    if (flipY)
        flipAxis = (SDL_RendererFlip)(flipAxis | SDL_FLIP_VERTICAL);

    if (math::Vec2{srcRect.x, srcRect.y} == math::Vec2())
    {
        SDL_RenderCopyExF(_renderer, texture.getSDLTexture(), nullptr, &dstRect, angle, nullptr,
                          flipAxis);
        return;
    }

    SDL_Rect src;
    src.x = (int)srcRect.x;
    src.y = (int)srcRect.y;
    src.w = (int)srcRect.w;
    src.h = (int)srcRect.h;

    SDL_RenderCopyExF(_renderer, texture.getSDLTexture(), &src, &dstRect, angle, nullptr, flipAxis);
}

void blitEx(const Texture& texture, const math::Vec2& position, double angle, bool flipX,
            bool flipY)
{
    if (!_renderer)
        WARN("Cannot blit before creating the window")

    SDL_RendererFlip flipAxis = SDL_FLIP_NONE;
    if (flipX)
        flipAxis = (SDL_RendererFlip)(flipAxis | SDL_FLIP_HORIZONTAL);
    if (flipY)
        flipAxis = (SDL_RendererFlip)(flipAxis | SDL_FLIP_VERTICAL);

    Rect rect = {(float)position.x, (float)position.y, (float)texture.getSize().x,
                 (float)texture.getSize().y};

    SDL_RenderCopyExF(_renderer, texture.getSDLTexture(), nullptr, &rect, angle, nullptr, flipAxis);
}

SDL_Renderer* getRenderer()
{
    if (!_renderer)
        WARN("Cannot get renderer before creating the window")

    return _renderer;
}

bool getFullscreen()
{
    if (!_window)
        WARN("Cannot get fullscreen before creating the window")

    return SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN;
}

int getScale()
{
    if (!_window)
        WARN("Cannot get scale before creating the window");

    float scale;
    SDL_RenderGetScale(_renderer, &scale, nullptr);

    return (int)scale;
}

void setTitle(const std::string& newTitle)
{
    if (!_window)
        WARN("Cannot set title before creating the window")

    if (newTitle.empty())
    {
        WARN("Cannot set title to empty string")
        return;
    }

    if (newTitle.size() > 255)
    {
        WARN("Cannot set title to string longer than 255 characters")
        return;
    }

    SDL_SetWindowTitle(_window, newTitle.c_str());
}

std::string getTitle()
{
    if (!_window)
        WARN("Cannot get title before creating the window")

    return {SDL_GetWindowTitle(_window)};
}

void setFullscreen(bool fullscreen)
{
    if (!_window)
        WARN("Cannot set fullscreen before creating the window")

    SDL_SetWindowFullscreen(_window, fullscreen);
}

math::Vec2 getSize()
{
    if (!_window)
        WARN("Cannot get size before creating the window")

    int w, h;
    SDL_RenderGetLogicalSize(_renderer, &w, &h);
    if (!w || !h)
        SDL_GetWindowSize(_window, &w, &h);

    return {w, h};
}
} // namespace kn::window
