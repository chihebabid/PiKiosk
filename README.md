# PiKiosk

A lightweight kiosk application developed in C++/SDL for Raspberry Pi. Displays photos, sensor readings (DHT11), current time and other realtime information in fullscreen.
It is not designed for production use, but rather as a personal project to learn C++ and SDL2.

## Features
- Fullscreen image slideshow (./images directory by default)
- Temperature and humidity display (DHT11)
- Time display
- Uses SDL2, SDL2_image and SDL2_ttf for rendering
- GPIO access via libgpiod (included helper library in libraries/gpiotools)

## Requirements
- Raspberry Pi (any model supported by libgpiod; RPi1 may need compatibility flags)
- CMake >= 3.20
- A C++20-capable toolchain (g++)
- System packages: libsdl2-dev, libsdl2-image-dev, libsdl2-ttf-dev, libgpiod-dev
- DHT11 sensor connected to GPIO pin 4 (or modify the code for a different pin)
- Can be run without X11 or Wayland, but requires a framebuffer (e.g., /dev/fb0) and a running SDL2 environment.

## Build
From the project root:

mkdir -p build && cd build
cmake ..
make -j

This produces the `kiosk` executable in the build folder.

## Run
By default the app looks for images in `./images` and uses GPIO pin 4 for the DHT11 sensor.

sudo env SDL_VIDEODRIVER=kmsdrm ./kiosk

Notes:
- Run with sufficient privileges or ensure the current user has access to GPIO devices (libgpiod).
- To change the images directory, modify the path passed to FactoryDisplay in `src/main.cpp` or run the binary from a directory containing an `images/` folder.
