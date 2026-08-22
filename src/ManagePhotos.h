//
// Created by chiheb on 16/08/2026.
//

#ifndef SDL_TEST_MANAGEPHOTOS_H
#define SDL_TEST_MANAGEPHOTOS_H
#include "ManageDisplay.h"
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class ManagePhotos : public ManageDisplay {
public:
    ManagePhotos(const std::string& path_to_images="./");
    auto display(SDL_Renderer *renderer) -> void override;
    auto next() -> void;
    auto init() -> void override;
    void transition(SDL_Renderer *renderer);
private:
    void runSlideshow(SDL_Renderer *renderer, SDL_Texture *texA, SDL_Texture *texB);
    SDL_Surface *resizeSurface(SDL_Surface *source, int maxWidth, int maxHeight);
    std::string path_to_images_;
    std::vector<fs::path> l_images_;
    size_t current_image_index_ {};

};


#endif //SDL_TEST_MANAGEPHOTOS_H
