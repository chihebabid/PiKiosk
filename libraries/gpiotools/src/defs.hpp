//
// Created by chiheb on 23/04/24.
//

#ifndef SMARTHOME_DEFS_HPP
#define SMARTHOME_DEFS_HPP
#include <string>
#include <gpiod.hpp>

namespace pitools {
    long map(long value, long fromLow, long fromHigh, long toLow, long toHigh);

    class GpioManager {
    public:
        static GpioManager &getInstance();

        GpioManager(const GpioManager &) = delete;

        GpioManager &operator=(const GpioManager &) = delete;

        GpioManager(GpioManager &&) = delete;

        GpioManager &operator=(GpioManager &&) = delete;


        gpiod::chip &getChip();


        // Diagnostic information
        std::string getChipName() const;

        std::string getChipLabel() const;

        std::unique_ptr<gpiod::line_request> requestOutputPin(unsigned int pin, gpiod::line::value initVal, const std::string &consumer);

    private:
        GpioManager();

        ~GpioManager() = default;

        // Auto-detects the main Broadcom/RP1 GPIO controller
        gpiod::chip detectAndOpenChip();

        gpiod::chip mChip;
        std::string mName;
        std::string mLabel;
    };
}
#endif //SMARTHOME_DEFS_HPP
