#include "Font.hpp"
#include "ErrorLogger.hpp"
#include "RenderWindow.hpp"

namespace kn
{

Font::Font(const std::string& fileDir, int ptSize)
{
    font = TTF_OpenFont(fileDir.c_str(), ptSize);
    if (!font)
        FATAL("Failed to load font: " + std::string(TTF_GetError()));
}

std::shared_ptr<Texture> Font::render(const std::string& text, bool antialias, Color color,
                                      int wrapLength)
{
    SDL_Surface* surface;

    if (antialias)
        if (wrapLength > 0)
            surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, wrapLength);
        else
            surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    else if (wrapLength > 0)
        surface = TTF_RenderText_Solid_Wrapped(font, text.c_str(), color, wrapLength);
    else
        surface = TTF_RenderText_Solid(font, text.c_str(), color);

    if (!surface)
        FATAL("Failed to render text: " + std::string(TTF_GetError()));

    SDL_Texture* sdlTexture =
        SDL_CreateTextureFromSurface(RenderWindow::getInstance().getRenderer(), surface);
    if (!sdlTexture)
        FATAL("Failed to create texture: " + std::string(SDL_GetError()));

    std::shared_ptr<Texture> texture = std::make_shared<Texture>(sdlTexture);
    SDL_FreeSurface(surface);

    return texture;
}

} // namespace kn
