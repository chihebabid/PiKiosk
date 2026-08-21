#include "ManagePhotos.h"
#include "ManageDisplay.h"
#include "ManageTemp.h"
#include "sensors/dht11.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>


namespace fs = std::filesystem;


auto init() -> void {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        exit(1);
    }
}


class ManageSDL {
public:
    // 1. Supprimer le constructeur de copie et l'opérateur d'affectation
    ManageSDL(const ManageSDL &) = delete;

    ManageSDL &operator=(const ManageSDL &) = delete;

    static ManageSDL &getInstance() {
        static ManageSDL instance;
        return instance;
    }

    static auto create() -> void {
        window_ = SDL_CreateWindow("SDL Image Viewer", 800, 600, 0);
        if (!window_) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
            exit(1);
        }

        renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (!renderer_) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
            exit(1);
        }
    }

    SDL_Renderer *getRenderer() const {
        return renderer_;
    }

private:
    ManageSDL() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
            exit(1);
        }


    }

    // 4. Destructeur privé : empêche la destruction manuelle (ex: delete &s;)
    ~ManageSDL() {
        if (renderer_) SDL_DestroyRenderer(renderer_);
        if (window_) SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    static SDL_Window *window_;
    static SDL_Renderer *renderer_;
};

SDL_Window *ManageSDL::window_{};
SDL_Renderer *ManageSDL::renderer_{};

auto myDelay(int milliseconds) -> bool {
    bool running = true;
    const auto start = std::chrono::steady_clock::now();
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT)
                running = false;
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE)
                running = false;
        }

        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        if (elapsed.count() >= milliseconds)
            break;
        SDL_Delay(10);
    }
    return running;
}
auto main() -> int {
    std::shared_ptr<ManagePhotos> managePhotos=FactoryDisplay::create<ManagePhotos>("./images");
    std::shared_ptr<ManageTemp> manageTemp=FactoryDisplay::create<ManageTemp>();

    ManageSDL &sdlManager{ManageSDL::getInstance()};
    sdlManager.create();
    SDL_SetRenderDrawBlendMode(sdlManager.getRenderer(), SDL_BLENDMODE_BLEND);

    pitools::sensors::DHT11 dht11(4);
    // ------------------------------------------------------------
    // Boucle principale
    // ------------------------------------------------------------
    auto renderer {sdlManager.getRenderer()};
    bool running = true;
    std::size_t imageIndex = 0;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                running = false;
        }
        managePhotos->display(renderer);
        SDL_RenderPresent(renderer);
        running=myDelay(2000);
        managePhotos->display(renderer);
        manageTemp->display(renderer);
        SDL_RenderPresent(renderer);
        running=myDelay(2500);
        managePhotos->display(renderer);
        manageTemp->display(renderer);
        manageTemp->displayTime(renderer);
        SDL_RenderPresent(renderer);
        running=myDelay(2500);
        managePhotos->display(renderer);
        SDL_RenderPresent(renderer);
        running=myDelay(2000);
        managePhotos->next();
    }
    return 0;
}
