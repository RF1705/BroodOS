#include "pixel_font.hpp"

#include <array>
#include <cctype>

namespace broodos {
namespace {

using Glyph = std::array<unsigned char, 7>;

Glyph glyph_for(char input) {
    const char character = static_cast<char>(std::toupper(static_cast<unsigned char>(input)));
    switch (character) {
        case 'A': return {{14,17,17,31,17,17,17}};
        case 'B': return {{30,17,17,30,17,17,30}};
        case 'C': return {{14,17,16,16,16,17,14}};
        case 'D': return {{30,17,17,17,17,17,30}};
        case 'E': return {{31,16,16,30,16,16,31}};
        case 'F': return {{31,16,16,30,16,16,16}};
        case 'G': return {{14,17,16,23,17,17,15}};
        case 'H': return {{17,17,17,31,17,17,17}};
        case 'I': return {{14,4,4,4,4,4,14}};
        case 'J': return {{7,2,2,2,2,18,12}};
        case 'K': return {{17,18,20,24,20,18,17}};
        case 'L': return {{16,16,16,16,16,16,31}};
        case 'M': return {{17,27,21,21,17,17,17}};
        case 'N': return {{17,25,21,19,17,17,17}};
        case 'O': return {{14,17,17,17,17,17,14}};
        case 'P': return {{30,17,17,30,16,16,16}};
        case 'Q': return {{14,17,17,17,21,18,13}};
        case 'R': return {{30,17,17,30,20,18,17}};
        case 'S': return {{15,16,16,14,1,1,30}};
        case 'T': return {{31,4,4,4,4,4,4}};
        case 'U': return {{17,17,17,17,17,17,14}};
        case 'V': return {{17,17,17,17,17,10,4}};
        case 'W': return {{17,17,17,21,21,21,10}};
        case 'X': return {{17,17,10,4,10,17,17}};
        case 'Y': return {{17,17,10,4,4,4,4}};
        case 'Z': return {{31,1,2,4,8,16,31}};
        case '0': return {{14,17,19,21,25,17,14}};
        case '1': return {{4,12,4,4,4,4,14}};
        case '2': return {{14,17,1,2,4,8,31}};
        case '3': return {{30,1,1,14,1,1,30}};
        case '4': return {{2,6,10,18,31,2,2}};
        case '5': return {{31,16,16,30,1,1,30}};
        case '6': return {{14,16,16,30,17,17,14}};
        case '7': return {{31,1,2,4,8,8,8}};
        case '8': return {{14,17,17,14,17,17,14}};
        case '9': return {{14,17,17,15,1,1,14}};
        case '.': return {{0,0,0,0,0,12,12}};
        case ':': return {{0,12,12,0,12,12,0}};
        case '/': return {{1,2,2,4,8,8,16}};
        case '\\': return {{16,8,8,4,2,2,1}};
        case '-': return {{0,0,0,31,0,0,0}};
        case '_': return {{0,0,0,0,0,0,31}};
        case '(': return {{2,4,8,8,8,4,2}};
        case ')': return {{8,4,2,2,2,4,8}};
        case '?': return {{14,17,1,2,4,0,4}};
        case '!': return {{4,4,4,4,4,0,4}};
        case '+': return {{0,4,4,31,4,4,0}};
        default: return {{0,0,0,0,0,0,0}};
    }
}

} // namespace

void draw_text(SDL_Renderer* renderer,
               int x,
               int y,
               const std::string& text,
               int scale,
               SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int cursor_x = x;
    int cursor_y = y;

    for (char character : text) {
        if (character == '\n') {
            cursor_x = x;
            cursor_y += 9 * scale;
            continue;
        }
        const Glyph glyph = glyph_for(character);
        for (int row = 0; row < 7; ++row) {
            for (int column = 0; column < 5; ++column) {
                if ((glyph[row] & (1 << (4 - column))) != 0) {
                    SDL_Rect pixel {cursor_x + column * scale, cursor_y + row * scale, scale, scale};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        cursor_x += 6 * scale;
    }
}

} // namespace broodos

