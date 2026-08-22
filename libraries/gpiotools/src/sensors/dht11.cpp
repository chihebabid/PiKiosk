/*
 * DHT11.cpp
 *
 *  Created on: 13 avr. 2021
 *      Author: chiheb
 */

#include "dht11.h"
#include "defs.hpp"
#include <thread>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <gpiod.hpp>


namespace pitools {
    namespace sensors {
        constexpr auto FREQUENCY = 10000000;

        DHT11::DHT11(const uint8_t gpioPin) : mDataPin(gpioPin) {
        }

        void DHT11::init() {
            gpiod::line_settings settings;
            settings.set_direction(gpiod::line::direction::OUTPUT)
                    .set_drive(gpiod::line::drive::OPEN_DRAIN)
                    .set_bias(gpiod::line::bias::PULL_UP)
                    .set_output_value(gpiod::line::value::ACTIVE);

            gpiod::line_config line_cfg;
            line_cfg.add_line_settings(mDataPin, settings);

            m_line_request_ = std::make_unique<gpiod::line_request>(
                pitools::GpioManager::getInstance().getChip().prepare_request()
                .set_consumer("DHT_Sensor")
                .set_line_config(line_cfg)
                .do_request()
            );
        }

        /*
        * Extracts the relevant data from the binary raw data.
        */
        std::optional<dht11_data_t> DHT11::ProcessData(uint64_t Data) {
            uint8_t HumidityHigh = (Data >> 32) & 0xFF;
            uint8_t HumidityLow = (Data >> 24) & 0xFF;
            uint8_t TemperatureHigh = (Data >> 16) & 0xFF;
            uint8_t TemperatureLow = (Data >> 8) & 0xFF;
            uint8_t Parity = Data & 0xFF;

            if (Parity
                != CalculateParity(HumidityHigh, HumidityLow, TemperatureHigh,
                                   TemperatureLow)) {
                mError = DHT11_ERRORS::CHECKSUM_ERROR;
                return std::nullopt;
            }
            return std::make_optional(dht11_data_t{TemperatureHigh, HumidityHigh});
        }

        std::optional<dht11_data_t> DHT11::getData() {
            uint64_t data{0};
            mError = DHT11_ERRORS::NO_ERROR;
            SendStartSignal();
            m_line_request_->set_value(mDataPin, gpiod::line::value::ACTIVE); //gpioSetMode(mDataPin, PI_INPUT);
            try {
                WaitForLow();
                WaitForHigh();
                WaitForLow();
                for (auto i{0}; i < 40; ++i) {
                    data <<= 1;
                    int LowTime = WaitForHigh();
                    int HighTime = WaitForLow();
                    if (LowTime < HighTime) {
                        data |= 0x1;
                    }
                }
                WaitForHigh();
            } catch (...) {
                m_line_request_->set_value(mDataPin, gpiod::line::value::ACTIVE);
                mError = DHT11_ERRORS::TIMEOUT;
                return std::nullopt;
            }
            m_line_request_->set_value(mDataPin, gpiod::line::value::ACTIVE);
            return ProcessData(data);
        }

        inline uint32_t getMicros() {
            using namespace std::chrono;
            return static_cast<uint32_t>(
                duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count()
            );
        }

        int DHT11::WaitForLow() {
            auto StartTime = getMicros();
            while (m_line_request_->get_value(mDataPin) == gpiod::line::value::ACTIVE) {
                if (FREQUENCY < getMicros() - StartTime) {
                    throw std::runtime_error(
                        "Timeout while waiting for pin to get low.");
                }
            }
            return getMicros() - StartTime;
        }


        // Then your function stays almost identical:
        int DHT11::WaitForHigh() {
            auto startTime {getMicros()};

            while (m_line_request_->get_value(mDataPin) == gpiod::line::value::INACTIVE) {
                if (FREQUENCY < getMicros() - startTime) {
                    throw std::runtime_error("Timeout while waiting for pin to get high.");
                }
            }

            return static_cast<int>(getMicros() - startTime);
        }

        void DHT11::SendStartSignal() {
            m_line_request_->set_value(mDataPin, gpiod::line::value::INACTIVE);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            m_line_request_->set_value(mDataPin, gpiod::line::value::ACTIVE);
        }


        uint8_t DHT11::CalculateParity(uint8_t HumidityHigh, uint8_t HumidityLow,
                                       uint8_t TemperatureHigh, uint8_t TemperatureLow) {
            return static_cast<uint8_t>(HumidityHigh + HumidityLow + TemperatureHigh
                                        + TemperatureLow);
        }

        DHT11_ERRORS DHT11::getError() {
            return mError;
        }
    } // end namespace sensors
} // end namespace pitools
