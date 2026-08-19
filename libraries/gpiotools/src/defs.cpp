//
// Created by chiheb on 23/04/24.
//
#include <stdexcept>
#include <array>
#include "defs.hpp"

namespace pitools {
    long map(long value, long fromLow, long fromHigh, long toLow, long toHigh) {
        return (toHigh - toLow) * (value - fromLow) / (fromHigh - fromLow) + toLow;
    }


    GpioManager &GpioManager::getInstance() {
        // Guaranteed to be thread-safe in C++11 and later
        static GpioManager instance;
        return instance;
    }

    GpioManager::GpioManager() : mChip(detectAndOpenChip()) {
        try {
            mName = mChip.get_info().name(); // e.g., "gpiochip0"
            mLabel = mChip.get_info().label(); // e.g., "pinctrl-bcm2835"
            std::cout << "[GpioManager] Successfully initialized GPIO chip: "
                    << mName << " (" << mLabel << ")\n";
        } catch (const std::exception &e) {
            std::cerr << "[GpioManager Error] Failed to initialize chip: " << e.what() << '\n';
            throw;
        }
    }

    gpiod::chip GpioManager::detectAndOpenChip() {
        // Liste des puces potentielles selon le Raspberry Pi / OS
        const std::vector<std::string> candidateChips = {
            "gpiochip0", // Standard Raspberry Pi 3 / 4
            "gpiochip4", // Standard Raspberry Pi 5
            "/dev/gpiochip0",
            "/dev/gpiochip4"
        };

        for (const auto &chipName: candidateChips) {
            try {
                gpiod::chip chip(chipName);
                return chip;
            } catch (...) {
                // Essaie la puce suivante si celle-ci n'existe pas
            }
        }

        throw std::runtime_error("Impossible de trouver une puce GPIO valide (/dev/gpiochip*)");
    }

    gpiod::chip &GpioManager::getChip() {
        return mChip;
    }

    std::string GpioManager::getChipName() const {
        return mName;
    }

    std::string GpioManager::getChipLabel() const {
        return mLabel;
    }

    // In your GpioManager / GPIODevice class:
    std::unique_ptr<gpiod::line_request> GpioManager::requestOutputPin(unsigned int pin, gpiod::line::value initVal,
                                                                       const std::string &consumer) {
        gpiod::line_config cfg;
        cfg.add_line_settings(pin, gpiod::line_settings()
                              .set_direction(gpiod::line::direction::OUTPUT)
                              .set_output_value(initVal));

        return std::make_unique<gpiod::line_request>(
            getChip().prepare_request().set_consumer(consumer).set_line_config(cfg).do_request()
        );
    }
}
