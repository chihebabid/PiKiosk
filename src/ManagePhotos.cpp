//
// Created by chiheb on 16/08/2026.
//

#include "ManagePhotos.h"
#include <SDL2/SDL_image.h>
#include <iostream>

ManagePhotos::ManagePhotos(const std::string &path_to_images) : path_to_images_(path_to_images) {
}


auto ManagePhotos::init() -> void {
    auto isImage = [](const fs::path &path) {
        if (!fs::is_regular_file(path))
            return false;

        const std::string ext = path.extension().string();

        return ext == ".jpg" || ext == ".JPG" || ext == ".jpeg" || ext == ".JPEG" || ext == ".png" || ext == ".PNG" || ext == ".bmp" || ext
               == ".BMP";
    };

    l_images_.clear();
    for (const auto &entry: fs::directory_iterator(path_to_images_)) {
        if (isImage(entry.path()))
            l_images_.push_back(entry.path());
    }
    if (l_images_.empty()) {
        std::cerr << "Aucune image trouvee dans " << path_to_images_ << '\n';
    }
}
auto ManagePhotos::next() -> void {
    current_image_index_ = (current_image_index_ + 1) % l_images_.size();
}

auto ManagePhotos::display(SDL_Renderer *renderer) -> void {
    SDL_Texture *texture{};

    std::cout << "Affichage : " << l_images_[current_image_index_] << '\n';
    SDL_Surface *surface{IMG_Load(l_images_[current_image_index_].c_str())};

    if (!surface) {
        std::cerr << "IMG_Load failed: " << IMG_GetError() << '\n';
        current_image_index_ = (current_image_index_ + 1) % l_images_.size();
        return;
    }

    SDL_RendererInfo rendererInfo{};
    SDL_GetRendererInfo(renderer, &rendererInfo);
    int maxTextureWidth{rendererInfo.max_texture_width};
    int maxTextureHeight{rendererInfo.max_texture_height};

    SDL_Surface *resizedSurface = resizeSurface(surface, maxTextureWidth, maxTextureHeight);
    SDL_FreeSurface(surface);
    if (!resizedSurface) {
        std::cerr << "Impossible de redimensionner l'image\n";
        current_image_index_ = (current_image_index_ + 1) % l_images_.size();
        return;
    }


    texture = SDL_CreateTextureFromSurface(renderer, resizedSurface);
    SDL_FreeSurface(resizedSurface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << '\n';
        current_image_index_ = (current_image_index_ + 1) % l_images_.size();
        return;
    }

    // --------------------------------------------------------
    // Récupérer dimensions image
    // --------------------------------------------------------

    int imageWidth, imageHeight;

    SDL_QueryTexture(texture, nullptr, nullptr, &imageWidth, &imageHeight);

    // --------------------------------------------------------
    // Adapter l'image à la fenêtre en conservant le ratio
    // --------------------------------------------------------

    int windowWidth, windowHeight;

    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    const double scaleX = static_cast<double>(windowWidth) / imageWidth;
    const double scaleY = static_cast<double>(windowHeight) / imageHeight;
    const double scale = std::min(scaleX, scaleY);
    const int dstWidth = static_cast<int>(imageWidth * scale);
    const int dstHeight = static_cast<int>(imageHeight * scale);
    SDL_Rect destination{(windowWidth - dstWidth) / 2, (windowHeight - dstHeight) / 2, dstWidth, dstHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, &destination);
    SDL_DestroyTexture(texture);
}

SDL_Surface *ManagePhotos::resizeSurface(SDL_Surface *source, int maxWidth, int maxHeight) {
    if (!source) return nullptr;
    const double scaleX{static_cast<double>(maxWidth) / source->w};
    const double scaleY{static_cast<double>(maxHeight) / source->h};
    const double scale{std::min(scaleX, scaleY)};

    if (scale >= 1.0)
        return SDL_ConvertSurface(source, source->format, 0);

    const int newWidth{static_cast<int>(source->w * scale)};
    const int newHeight{static_cast<int>(source->h * scale)};

    SDL_Surface *resized = SDL_CreateRGBSurfaceWithFormat(0, newWidth, newHeight, 32, SDL_PIXELFORMAT_RGBA32);
    if (!resized)
        return nullptr;

    SDL_Rect destination{0, 0, newWidth, newHeight};
    if (SDL_BlitScaled(source, nullptr, resized, &destination) != 0) {
        SDL_FreeSurface(resized);
        return nullptr;
    }
    return resized;
}
