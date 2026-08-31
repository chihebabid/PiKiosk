//
// Created by chiheb on 16/08/2026.
//

#include "ManageTemp.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <iostream>
#include <chrono>
#include <format>

auto ManageTemp::displayTime(SDL_Renderer *renderer) -> void {
    static uint8_t _r{80}, _g{100}, _b{40};
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, 150);
    _r = (_r + 2) % 255;
    _g = (_g + 5) % 255;
    _b = (_b + 4) % 255;

    SDL_FRect fullScreenRect{380.0f, 80.0f, 400.0f, 140.0f};
    SDL_RenderFillRect(renderer, &fullScreenRect);

    if (!clock_icon_) {
        clock_icon_ = IMG_LoadTexture(renderer, "./icons/time.jpg");
        if (!clock_icon_) {
            std::cerr << "Erreur time.jpg: " << SDL_GetError() << '\n';
        }
    }


    const auto now = std::chrono::system_clock::now();
    const std::string currentTime = std::format("{:%H:%M}", now);

    renderSensorValue(renderer, font_, clock_icon_, currentTime, 400, 100);
}

auto ManageTemp::display(SDL_Renderer *renderer) -> void {
    static uint8_t _r{20}, _g{30}, _b{180};
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, 150);
    _r = (_r + 3) % 255;
    _g = (_g + 2) % 255;
    _b = (_b + 1) % 255;

    SDL_FRect fullScreenRect{80.0f, 280.0f, 400.0f, 280.0f};
    SDL_RenderFillRect(renderer, &fullScreenRect);

    if (!temperature_icon_) {
        temperature_icon_ = IMG_LoadTexture(renderer, "./icons/temp.jpg");
        if (!temperature_icon_) {
            std::cerr << "Erreur temp.jpg: " << SDL_GetError() << '\n';
        }
    }
    if (!humidity_icon_) {
        humidity_icon_ = IMG_LoadTexture(renderer, "./icons/humidity.jpg");
        if (!humidity_icon_) {
            std::cerr << "Erreur humidity.jpg: " << SDL_GetError() << '\n';
        }
    }

    const auto res = dht11_->getData();
    if (res.has_value()) {
        temp_ = res->temperature;
        humidity_ = res->humidity;
    } else {
        std::cerr << "Erreur lecture DHT11\n";
    }

    const std::string temperatureText{std::format("{:.2f} °C", static_cast<float>(temp_))};
    const std::string humidityText{std::format("{:.2f} %", static_cast<float>(humidity_))};

    renderSensorValue(renderer, font_, temperature_icon_, temperatureText, 100, 300);
    renderSensorValue(renderer, font_, humidity_icon_, humidityText, 100, 440);
}

auto ManageTemp::init() -> void {
    if (!TTF_Init()) {
        std::cerr << "TTF_Init failed: " << SDL_GetError() << '\n';
        exit(1);
    }

    font_ = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 40.0f);
    if (!font_) {
        std::cerr << "TTF_OpenFont failed: " << SDL_GetError() << '\n';
        exit(1);
    }

    dht11_ = std::make_unique<pitools::sensors::DHT11>(4);
    dht11_->init();
}

ManageTemp::~ManageTemp() {
    if (temperature_icon_) {
        SDL_DestroyTexture(temperature_icon_);
    }
    if (humidity_icon_) {
        SDL_DestroyTexture(humidity_icon_);
    }
    if (gas_icon_) {
        SDL_DestroyTexture(gas_icon_);
    }
    if (font_) {
        TTF_CloseFont(font_);
    }
    TTF_Quit();
}

auto ManageTemp::renderSensorValue(SDL_Renderer *renderer, TTF_Font *font, SDL_Texture *icon, const std::string &value, int x, int y) ->
    void {
    if (!renderer or !font or !icon) {
        std::cerr << "Renderer, font, or icon is null. Cannot render sensor value." << std::endl;
        return;
    }

    float iconWidth{}, iconHeight{};
    SDL_GetTextureSize(icon, &iconWidth, &iconHeight);

    // Taille d'affichage de l'icône
    constexpr float displayIconWidth{160.0f}, displayIconHeight{100.0f};
    SDL_FRect iconRect{static_cast<float>(x), static_cast<float>(y), displayIconWidth, displayIconHeight};
    SDL_RenderTexture(renderer, icon, nullptr, &iconRect);

    const SDL_Color textColor{255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, value.c_str(), value.length(), textColor);
    if (!textSurface) {
        std::cerr << "TTF_RenderText_Blended failed: " << SDL_GetError() << '\n';
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << '\n';
        SDL_DestroySurface(textSurface);
        return;
    }

    const float textWidth{static_cast<float>(textSurface->w)};
    const float textHeight{static_cast<float>(textSurface->h)};
    const float spacing{20.0f};

    SDL_FRect textRect{x + displayIconWidth + spacing, y + (displayIconHeight - textHeight) / 2.0f, textWidth, textHeight};
    SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);

    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

auto ManageTemp::displayGas(SDL_Renderer *renderer) -> void {
    constexpr uint8_t _r{80}, _g{200}, _b{40};
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, 150);
    SDL_FRect rect{80.0f, 280.0f, 400.0f, 280.0f};
    SDL_RenderFillRect(renderer, &rect);

    if (!gas_icon_) {
        gas_icon_ = IMG_LoadTexture(renderer, "./icons/gas.png");
        if (!gas_icon_) {
            std::cerr << "Erreur gas.png: " << SDL_GetError() << '\n';
        }
    }

    const std::string gasText {"Concentration de méthane : "};
    renderSensorValue(renderer, font_, temperature_icon_, gasText, 100, 300);

}