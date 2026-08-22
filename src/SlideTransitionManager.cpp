//
// Created by chiheb on 22/08/2026.
//

#include "SlideTransitionManager.h"
#include <algorithm>

void SlideTransitionManager::cross_fading(SDL_Renderer *renderer, SDL_Texture *currentTexture, SDL_Texture *nextTexture, float progress) {
    progress = std::clamp(progress, 0.0f, 1.0f);

    // 1. Enable Alpha Blending on both textures
    SDL_SetTextureBlendMode(currentTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(nextTexture, SDL_BLENDMODE_BLEND);

    SDL_SetTextureAlphaModFloat(currentTexture, 1.0f - progress);
    SDL_SetTextureAlphaModFloat(nextTexture, progress);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    auto renderScaled = [&](SDL_Texture *tex) {
        if (!tex) return;
        float imgW = 0.0f, imgH = 0.0f;
        SDL_GetTextureSize(tex, &imgW, &imgH);
        if (imgW <= 0 || imgH <= 0) return;

        const float scale = std::min(windowWidth / imgW, windowHeight / imgH);
        const float dstW = imgW * scale;
        const float dstH = imgH * scale;

        SDL_FRect dstRect{
            (windowWidth - dstW) / 2.0f,
            (windowHeight - dstH) / 2.0f,
            dstW,
            dstH
        };
        SDL_RenderTexture(renderer, tex, nullptr, &dstRect);
    };

    renderScaled(currentTexture);
    renderScaled(nextTexture);

    SDL_RenderPresent(renderer);
}
