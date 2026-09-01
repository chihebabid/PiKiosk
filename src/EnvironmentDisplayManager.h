//
// Created by chiheb on 16/08/2026.
//

#ifndef SDL_TEST_MANAGETEMP_H
#define SDL_TEST_MANAGETEMP_H
#include "ManageDisplay.h"
#include "sensors/dht11.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class EnvironmentDisplayManager : public ManageDisplay {
public:

    auto display(SDL_Renderer *renderer) -> void override;

    auto displayTime(SDL_Renderer *renderer) -> void;

    auto displayGas(SDL_Renderer *renderer) -> void;

    auto init() -> void override;

    virtual ~EnvironmentDisplayManager() override;

private:
    void renderSensorValue(SDL_Renderer *renderer, TTF_Font *font, SDL_Texture *icon, const std::string &value, int x, int y,uint32_t
        icon_width=160, uint32_t icon_height=100,SDL_Color text_color={255,255,255,255});

    int temp_{};
    int humidity_{};
    TTF_Font *font_{};
    SDL_Texture *temperature_icon_{};
    SDL_Texture *humidity_icon_{};
    SDL_Texture *gas_icon_{};
    SDL_Texture *clock_icon_{};
    std::unique_ptr<pitools::sensors::DHT11> dht11_{};

};


#endif //SDL_TEST_MANAGETEMP_H
