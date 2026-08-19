//
// Created by chiheb on 16/08/2026.
//

#include "ManageTemp.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include <chrono>
#include <format>

auto ManageTemp::displayTime(SDL_Renderer *renderer) -> void {

    static int _r{80}, _g{100}, _b{40};
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, 150);
    _r = (_r + 2) % 255;
    _g = (_g + 5) % 255;
    _b = (_b + 4) % 255;
    SDL_Rect fullScreenRect{380, 80, 400, 140};
    SDL_RenderFillRect(renderer, &fullScreenRect);

    if (!clock_icon_) {
        clock_icon_ = IMG_LoadTexture(renderer, "./icons/time.jpg");
        if (!clock_icon_) {
            std::cerr << "Erreur time.jpg: " << IMG_GetError() << '\n';
        }
    }

    const SDL_Color textColor{255, 255, 255, 255};
    const auto now = std::chrono::system_clock::now();

    const std::string currentTime = std::format("{:%H:%M}", now);

    renderSensorValue(renderer, font_, clock_icon_, currentTime, 400, 100);
}
auto ManageTemp::display(SDL_Renderer *renderer) -> void {
    static int _r{20}, _g{30}, _b{180};
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, 150);
    _r = (_r + 3) % 255;
    _g = (_g + 2) % 255;
    _b = (_b + 1) % 255;
    SDL_Rect fullScreenRect{80, 280, 400, 280};
    SDL_RenderFillRect(renderer, &fullScreenRect);

    if (!temperature_icon_) {
        temperature_icon_ = IMG_LoadTexture(renderer, "./icons/temp.jpg");
        if (!temperature_icon_) {
            std::cerr << "Erreur temp.jpg: " << IMG_GetError() << '\n';
        }
    }
    if (!humidity_icon_) {
        humidity_icon_ = IMG_LoadTexture(renderer, "./icons/humidity.jpg");
        if (!humidity_icon_) {
            std::cerr << "Erreur humidity.jpg: " << IMG_GetError() << '\n';
        }
    }


    const auto [temperature, humidity] = dht11_->getData();

    const std::string temperatureText{std::format("{:.2f} °C", static_cast<float>(temperature))};
    const std::string humidityText{std::format("{:.2f} %", static_cast<float>(humidity))};

    renderSensorValue(renderer, font_, temperature_icon_, temperatureText, 100, 300);
    renderSensorValue(renderer, font_, humidity_icon_, humidityText, 100, 440);
}

auto ManageTemp::init() -> void {
    TTF_Init();
    font_ = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 40);
    if (!font_) {
        std::cerr << "TTF_OpenFont failed: " << TTF_GetError() << '\n';
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
    if (font_) {
        TTF_CloseFont(font_);
    }
}

void ManageTemp::renderSensorValue(SDL_Renderer *renderer, TTF_Font *font, SDL_Texture *icon, const std::string &value, int x, int y) {
    if (!renderer || !font || !icon) {
        std::cerr << "Renderer, font, or icon is null. Cannot render sensor value." << std::endl;
        std::cerr << "Renderer: " << renderer << ", Font: " << font << ", Icon: " << icon << std::endl;
        return;
    }

    int iconWidth,iconHeight;
    SDL_QueryTexture(icon, nullptr, nullptr, &iconWidth, &iconHeight);

    // Taille d'affichage de l'icône
    constexpr int displayIconWidth{160}, displayIconHeight{100};
    SDL_Rect iconRect{x, y, displayIconWidth, displayIconHeight};
    SDL_RenderCopy(renderer, icon, nullptr, &iconRect);

    const SDL_Color textColor{255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(font, value.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "TTF_RenderUTF8_Blended failed: " << TTF_GetError() << '\n';
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << '\n';
        SDL_FreeSurface(textSurface);
        return;
    }

    const int textWidth = textSurface->w;
    const int textHeight = textSurface->h;
    const int spacing = 20;
    SDL_Rect textRect;
    textRect.x = x + displayIconWidth + spacing;
    textRect.y = y + (displayIconHeight - textHeight) / 2;
    textRect.w = textWidth;
    textRect.h = textHeight;
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
