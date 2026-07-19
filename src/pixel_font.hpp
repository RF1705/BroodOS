#pragma once

#include <SDL.h>
#include <string>

namespace broodos {

void draw_text(SDL_Renderer* renderer,
               int x,
               int y,
               const std::string& text,
               int scale,
               SDL_Color color);

} // namespace broodos

