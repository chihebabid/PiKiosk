//
// Created by chiheb on 22/08/2026.
//

#include "SlideTransitionManager.h"
#include <algorithm>
#include <functional>

void SlideTransitionManager::crossFading(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture, float progress) {
    progress = std::clamp(progress, 0.0f, 1.0f);

    SDL_SetTextureBlendMode(currentTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(nextTexture, SDL_BLENDMODE_BLEND);

    SDL_SetTextureAlphaModFloat(currentTexture, 1.0f - progress);
    SDL_SetTextureAlphaModFloat(nextTexture, progress);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int windowWidth{}, windowHeight{};
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    auto renderScaled = [&](SDL_Texture *tex) {
        if (!tex) return;
        float imgW{}, imgH{};
        SDL_GetTextureSize(tex, &imgW, &imgH);
        if (imgW <= 0 || imgH <= 0) return;

        const float scale{std::min(windowWidth / imgW, windowHeight / imgH)};
        const float dstW{imgW * scale};
        const float dstH{imgH * scale};

        SDL_FRect dstRect{(windowWidth - dstW) / 2.0f, (windowHeight - dstH) / 2.0f, dstW, dstH};
        SDL_RenderTexture(renderer, tex, nullptr, &dstRect);
    };

    renderScaled(currentTexture);
    renderScaled(nextTexture);

    SDL_RenderPresent(renderer);
}

void SlideTransitionManager::wipingHTopToBottom(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture, float
    progress) {
    wiping(renderer,currentTexture,nextTexture,progress,false,false);
}
void SlideTransitionManager::wipingHBottomToTop(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture, float
    progress) {
    wiping(renderer,currentTexture,nextTexture,progress,false,true);
}
void SlideTransitionManager::wipingVLeftToRight(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture, float
    progress) {
    wiping(renderer,currentTexture,nextTexture,progress,true,false);
}
void SlideTransitionManager::wipingVRightToLeft(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture, float
    progress) {
    wiping(renderer,currentTexture,nextTexture,progress,true,true);
}

void SlideTransitionManager::wiping(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture, float progress,
                                    bool horizontal,bool reverse) {
    int direction {reverse ? -1 : 1};
    progress = std::clamp(progress, 0.0f, 1.0f);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int windowWidth{}, windowHeight{};
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);
    const float offset{progress * static_cast<float>(horizontal ? windowWidth : windowHeight)};

    auto renderPushed = [&](SDL_Texture *tex, float shift, bool horizontal) {
        if (!tex) return;
        float imgW{}, imgH{};
        SDL_GetTextureSize(tex, &imgW, &imgH);
        if (imgW <= 0 or imgH <= 0) return;

        const float scale{std::min(windowWidth / imgW, windowHeight / imgH)};
        const float dstW{imgW * scale};
        const float dstH{imgH * scale};

        SDL_FRect dstRect{
            ((windowWidth - dstW) / 2.0f) + (horizontal ? shift : 0.0f), (windowHeight - dstH) / 2.0f + (horizontal ? 0.0f :
                shift), dstW,
            dstH
        };
        SDL_RenderTexture(renderer, tex, nullptr, &dstRect);
    };

    renderPushed(currentTexture, direction*offset, horizontal);
    renderPushed(nextTexture, ((horizontal ? - static_cast<float>(windowWidth) : - static_cast<float>(windowHeight)) + offset)*direction,
        horizontal);
    SDL_RenderPresent(renderer);
}


void SlideTransitionManager::runSlideshow(SDL_Renderer *renderer, SDL_Texture *texA, SDL_Texture *texB) {
    bool running = true;
    constexpr float transitionDurationSec = 1.5f; // 1.5 seconds fade
    const uint64_t startTicks = SDL_GetTicks();
    static int transition_mode{};
    using TransitionFunc = void(*)(SDL_Renderer *, SDL_Texture *, SDL_Texture *, float);
    TransitionFunc liste_modes[] = {
        &SlideTransitionManager::crossFading, &SlideTransitionManager::wipingHTopToBottom, &SlideTransitionManager::wipingHBottomToTop,
        &SlideTransitionManager::wipingVLeftToRight,&SlideTransitionManager::wipingVRightToLeft
    };
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        float elapsedSeconds = (SDL_GetTicks() - startTicks) / 1000.0f;
        float progress = elapsedSeconds / transitionDurationSec;
        liste_modes[transition_mode](renderer, texA, texB, progress);
        if (progress >= 1.0f) {
            break;
        }
        SDL_Delay(16); // ~60 FPS
    }
    transition_mode = (transition_mode + 1) % (sizeof(liste_modes) / sizeof(liste_modes[0]));
}
