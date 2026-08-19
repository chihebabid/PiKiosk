cmake_minimum_required(VERSION 3.20)


find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

find_path(GPIOD_INCLUDE_DIR
        NAMES gpiod.h
)

find_library(GPIOD_LIBRARY
        NAMES gpiod
)

add_executable(kiosk
    main.cpp ManagePhotos.cpp ManageTemp.cpp
)

target_link_libraries(kiosk
    PRIVATE
    SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf
)

# Liaison statique de libstdc++ et ajout de libatomic pour la compatibilité avec RPI1
target_link_options(kiosk PRIVATE
 #      -march=armv6
 #       -mfpu=vfp
 #       -mfloat-abi=hard
        -static-libstdc++
)



target_link_libraries(kiosk PRIVATE atomic pitools ${GPIOD_LIBRARY})

target_link_libraries(kiosk PUBLIC ${GPIOD_LIBRARY})