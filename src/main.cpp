#include "pixel_font.hpp"
#include "storage_scan.hpp"

#include <SDL.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr int kCanvasWidth = 1280;
constexpr int kCanvasHeight = 720;

std::string argument_value(int argc, char* argv[], const std::string& option) {
    for (int index = 1; index + 1 < argc; ++index) {
        if (argv[index] == option) {
            return argv[index + 1];
        }
    }
    return {};
}

bool has_argument(int argc, char* argv[], const std::string& option) {
    for (int index = 1; index < argc; ++index) {
        if (argv[index] == option) {
            return true;
        }
    }
    return false;
}

std::string shortened(const std::string& value, std::size_t maximum) {
    if (value.size() <= maximum) {
        return value;
    }
    return "..." + value.substr(value.size() - maximum + 3);
}

void fill_rect(SDL_Renderer* renderer, SDL_Rect rectangle, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rectangle);
}

void render(SDL_Renderer* renderer, const broodos::ScanResult& scan) {
    SDL_SetRenderDrawColor(renderer, 5, 10, 18, 255);
    SDL_RenderClear(renderer);

    fill_rect(renderer, {0, 0, kCanvasWidth, 10}, {75, 202, 255, 255});
    fill_rect(renderer, {48, 42, 1184, 108}, {10, 28, 45, 255});
    broodos::draw_text(renderer, 76, 65, "BROODOS", 7, {125, 222, 255, 255});
    broodos::draw_text(renderer, 78, 123, "UNOFFICIAL STARCRAFT PORT FOR LG WEBOS", 2, {157, 170, 187, 255});

    const bool found = !scan.locations.empty();
    const SDL_Color status_color = found ? SDL_Color{92, 224, 151, 255} : SDL_Color{255, 184, 77, 255};
    fill_rect(renderer, {48, 180, 1184, 130}, {12, 23, 35, 255});
    broodos::draw_text(renderer, 76, 207, found ? "GAME DATA FOUND" : "WAITING FOR ORIGINAL GAME DATA", 3, status_color);

    if (found) {
        const auto& location = scan.locations.front();
        broodos::draw_text(renderer, 76, 254, shortened(location.directory, 88), 2, {205, 216, 226, 255});
        broodos::draw_text(renderer, 980, 254, location.patch_rt.empty() ? "PATCH OPTIONAL" : "PATCH FOUND", 2,
                           location.patch_rt.empty() ? SDL_Color{157, 170, 187, 255} : SDL_Color{92, 224, 151, 255});
    } else {
        broodos::draw_text(renderer, 76, 254, "COPY STARDAT.MPQ AND BROODAT.MPQ TO USB/BROODOS", 2, {205, 216, 226, 255});
    }

    fill_rect(renderer, {48, 338, 1184, 256}, {12, 23, 35, 255});
    broodos::draw_text(renderer, 76, 363, "SEARCH PATHS", 3, {125, 222, 255, 255});
    int y = 410;
    for (std::size_t index = 0; index < scan.roots.size() && index < 6; ++index) {
        const bool inaccessible = std::find(scan.inaccessible_roots.begin(), scan.inaccessible_roots.end(), scan.roots[index]) !=
                                  scan.inaccessible_roots.end();
        broodos::draw_text(renderer, 78, y, inaccessible ? "-" : "+", 2,
                           inaccessible ? SDL_Color{123, 134, 148, 255} : SDL_Color{92, 224, 151, 255});
        broodos::draw_text(renderer, 110, y, shortened(scan.roots[index], 88), 2,
                           inaccessible ? SDL_Color{123, 134, 148, 255} : SDL_Color{205, 216, 226, 255});
        y += 29;
    }

    fill_rect(renderer, {48, 628, 1184, 54}, {10, 28, 45, 255});
    broodos::draw_text(renderer, 76, 647, "R OR OK: RESCAN    BACK OR ESC: EXIT", 2, {157, 170, 187, 255});
    SDL_RenderPresent(renderer);
}

} // namespace

int main(int argc, char* argv[]) {
    const std::string explicit_directory = argument_value(argc, argv, "--data-dir");
    std::vector<std::string> roots = broodos::default_scan_roots(explicit_directory);
    broodos::ScanResult scan = broodos::scan_for_game_data(roots);

    std::cout << "BroodOS 0.1.0\n";
    std::cout << "Scanned " << scan.roots.size() << " roots; found " << scan.locations.size() << " game data location(s).\n";

    if (has_argument(argc, argv, "--scan-only")) {
        for (const std::string& root : scan.roots) {
            std::cout << "scan root: " << root << '\n';
        }
        for (const auto& location : scan.locations) {
            std::cout << "game data: " << location.directory << '\n';
        }
        return scan.locations.empty() ? 2 : 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_Window* window = SDL_CreateWindow("BroodOS",
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           kCanvasWidth,
                                           kCanvasHeight,
                                           SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_RenderSetLogicalSize(renderer, kCanvasWidth, kCanvasHeight);

    bool running = true;
    bool redraw = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                const SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE || key == SDLK_BACKSPACE) {
                    running = false;
                } else if (key == SDLK_r || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                    scan = broodos::scan_for_game_data(roots);
                    redraw = true;
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                scan = broodos::scan_for_game_data(roots);
                redraw = true;
            }
        }

        if (redraw) {
            render(renderer, scan);
            redraw = false;
        }
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
