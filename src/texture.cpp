#include <SDL_image.h>

#include "Math.hpp"
#include "Texture.hpp"
#include "Window.hpp"

namespace kn
{

Texture::~Texture()
{
    if (texture)
        SDL_DestroyTexture(texture);
}

bool Texture::loadFromFile(const std::string& filePath)
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    texture = IMG_LoadTexture(window::getRenderer(), filePath.c_str());
    if (!texture)
    {
        WARN("Failed to create texture from: " + filePath);
        return false;
    }

    query();

    return true;
}

Texture::Texture(const math::Vec2& size, const Color color)
{
    SDL_Surface* surface =
        SDL_CreateRGBSurface(0, static_cast<int>(size.x), static_cast<int>(size.y), 32, 0, 0, 0, 0);
    if (!surface)
        WARN("Failed to create surface")
    else
        SDL_FillRect(surface, nullptr,
                     SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));

    texture = SDL_CreateTextureFromSurface(window::getRenderer(), surface);
    if (!texture)
        WARN("Failed to create texture from surface");

    SDL_FreeSurface(surface);

    query();
}

Texture::Texture(SDL_Texture* sdlTexture) : texture(sdlTexture) { query(); }

void Texture::query()
{
    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    this->rect = {0, 0, w, h};
}

math::Vec2 Texture::getSize() const { return {rect.w, rect.h}; }

Rect Texture::getRect() const { return rect; }

SDL_Texture* Texture::getSDLTexture() const { return texture; }

} // namespace kn
