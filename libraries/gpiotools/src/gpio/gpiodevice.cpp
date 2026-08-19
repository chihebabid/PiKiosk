//
// Created by chiheb on 25/04/24.
//
#include "gpiodevice.hpp"
#include "defs.hpp"
#include <gpiod.hpp>

namespace pitools {
    namespace gpio {
        GPIODevice::GPIODevice(const uint8_t &p):mPin(p)  {

        }

        GPIODevice& GPIODevice::mode(const pitools::gpio::GPIOMODE &m) {
            pitools::GpioManager& gpioManager = pitools::GpioManager::getInstance();
            gpiod::chip& chip = gpioManager.getChip();

            // 1. Prepare the line settings (direction and initial value)
            gpiod::line_settings settings;
            if (m == pitools::gpio::GPIOMODE::INPUT) {
                settings.set_direction(gpiod::line::direction::INPUT);
            } else {
                settings.set_direction(gpiod::line::direction::OUTPUT);
                settings.set_output_value(gpiod::line::value::INACTIVE); // LOW (0)
            }

            // 2. Map line settings to the target pin offset
            gpiod::line_config line_cfg;
            line_cfg.add_line_settings(mPin, settings);

            // 3. Reconfigure if line request is already active, otherwise request a new line
            if (mRequest) {
                mRequest->reconfigure_lines(line_cfg);
            } else {

                auto request = chip.prepare_request()
                       .set_consumer("GPIODevice")
                       .set_line_config(line_cfg)
                       .do_request();

                mRequest = std::make_unique<gpiod::line_request>(std::move(request));
            }

            return *this;
        }

        GPIOSTATE GPIODevice::state() const {
            return static_cast<GPIOSTATE>(mRequest->get_value(mPin) == gpiod::line::value::ACTIVE ? GPIOSTATE::HIGH : GPIOSTATE::LOW);
        }

        GPIODevice&  GPIODevice::set(const bool &b) {

            mRequest->set_value(mPin, b ? gpiod::line::value::ACTIVE : gpiod::line::value::INACTIVE);
            return *this;
        }
        GPIODevice& GPIODevice::clear() {
            mRequest->set_value(mPin, gpiod::line::value::INACTIVE);
            return *this;
        }

        GPIODevice& GPIODevice::use() {
            //
            return *this;
        }
        GPIODevice& GPIODevice::unuse() {
            return *this;
        }

        uint8_t GPIODevice::getPin() const {
            return mPin;
        }

    }
}