cmake_minimum_required(VERSION 3.20)


find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

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
    SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf
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