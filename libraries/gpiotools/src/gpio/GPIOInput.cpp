//
// Created by chiheb on 25/04/24.
//

#include "GPIOInput.hpp"
#include <stdexcept>
#include <gpiod.hpp>

namespace pitools {
    namespace gpio {
        GPIOInput::GPIOInput(const uint8_t &p) : GPIODevice(p) {
            mode(GPIOMODE::INPUT);
        }
        std::shared_ptr<GPIOInput> GPIOInput::create(const uint8_t &p) {
            return std::make_shared<GPIOInput>(p);
        }

        GPIOInput& GPIOInput::pullup() {
            gpiod::line_settings settings;
            settings.set_direction(gpiod::line::direction::INPUT);
            settings.set_bias(gpiod::line::bias::PULL_UP);

            // 2. Associer ces réglages au numéro de votre broche (mPin)
            gpiod::line_config line_cfg;
            line_cfg.add_line_settings(mPin, settings);

            // 3. Appliquer la nouvelle configuration à la requête existante
            mRequest->reconfigure_lines(line_cfg);
            return *this;
        }

        GPIOInput& GPIOInput::pulldown() {
            gpiod::line_settings settings;
            settings.set_direction(gpiod::line::direction::INPUT);
            settings.set_bias(gpiod::line::bias::PULL_DOWN);

            gpiod::line_config line_cfg;
            line_cfg.add_line_settings(mPin, settings);

            mRequest->reconfigure_lines(line_cfg);
            return *this;
        }

        GPIOInput& GPIOInput::pulloff() {
            gpiod::line_settings settings;
            settings.set_direction(gpiod::line::direction::INPUT);
            settings.set_bias(gpiod::line::bias::DISABLED);

            gpiod::line_config line_cfg;
            line_cfg.add_line_settings(mPin, settings);

            mRequest->reconfigure_lines(line_cfg);
            return *this;
        }


#include <gpiod.hpp>
#include <chrono>
#include <thread>
#include <stdexcept>

        GPIOInput& GPIOInput::waitUntil(const GPIOSTATE& state, uint32_t& duration, uint32_t timeout) {
            using clock = std::chrono::steady_clock;

            // Convertir l'état attendu vers le type v2 (gpiod::line::value)
            const gpiod::line::value target_value = (state == GPIOSTATE::HIGH)
                                                     ? gpiod::line::value::ACTIVE
                                                     : gpiod::line::value::INACTIVE;

            const auto start = clock::now();

            while (mRequest->get_value(mPin) != target_value) {
                if (timeout != 0) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                        clock::now() - start
                    ).count();

                    if (static_cast<uint32_t>(elapsed) >= timeout) {
                        throw std::runtime_error("timeout");
                    }
                }
                std::this_thread::yield();
            }

            // Calcul de la durée écoulée en microsecondes (équivalent à gpioTick())
            duration = static_cast<uint32_t>(
                std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start).count()
            );

            return *this;
        }
    }
}