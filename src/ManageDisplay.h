//
// Created by chiheb on 16/08/2026.
//

#ifndef SDL_TEST_MANAGEDISPLAY_H
#define SDL_TEST_MANAGEDISPLAY_H
#include <memory>
#include <SDL2/SDL.h>
class ManageDisplay {
public:
    virtual auto display(SDL_Renderer *renderer) -> void = 0;
    virtual auto init() -> void = 0;
    virtual ~ManageDisplay() = default;
};



class FactoryDisplay {
public:
    template <typename T, typename... Args>
    static std::shared_ptr<T> create(Args&&... args) {
        // Phase 1: Fully construct the derived object (Base -> Derived)
        auto instance = std::make_shared<T>(std::forward<Args>(args)...);
        instance->init();
        return instance;
    }
};
#endif //SDL_TEST_MANAGEDISPLAY_H
