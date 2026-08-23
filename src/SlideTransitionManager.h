//
// Created by chiheb on 22/08/2026.
//

#ifndef KIOSK_APP_SLIDETRANSITIONMANAGER_H
#define KIOSK_APP_SLIDETRANSITIONMANAGER_H

#include <SDL3/SDL.h>
#include <functional>

class SlideTransitionManager {
public:
    static void crossFading(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture,
                                float progress);

    static void wiping(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture,
                                float progress);

    static void runSlideshow(SDL_Renderer *renderer, SDL_Texture *texA, SDL_Texture *texB);

};
#endif //KIOSK_APP_SLIDETRANSITIONMANAGER_H
