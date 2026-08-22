//
// Created by chiheb on 22/08/2026.
//

#ifndef KIOSK_APP_SLIDETRANSITIONMANAGER_H
#define KIOSK_APP_SLIDETRANSITIONMANAGER_H

#include <SDL3/SDL.h>
class SlideTransitionManager {
public:
    static void cross_fading(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture,
                                float progress);

};
#endif //KIOSK_APP_SLIDETRANSITIONMANAGER_H
