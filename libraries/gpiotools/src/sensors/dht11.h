/*
 * DHT11.h
 *
 *  Created on: 13 avr. 2021
 *      Author: chiheb
 */

#ifndef CDHT11_H_
#define CDHT11_H_
#include "sensor.hpp"
#include <gpiod.hpp>

namespace pitools {
    namespace sensors {
        enum class DHT11_ERRORS {
            NO_ERROR = 0,
            TIMEOUT = 1,
            CHECKSUM_ERROR = 2,
            UNKNOWN_ERROR = 3
        };
        struct dht11_data_t {
            int temperature;
            int humidity;
        };
        class DHT11 : public Sensor<std::optional<dht11_data_t>> {
        public:
            /*
             * Constructor
             * Provide an instance to a raspberry pi object and the GPIO pin for data transmission.
             */
            explicit DHT11(const uint8_t gpioPin);

            std::optional<dht11_data_t> getData() override;

            void init() override;

        private:
            const uint8_t mDataPin;
            DHT11_ERRORS mError {DHT11_ERRORS::NO_ERROR};
            std::unique_ptr<gpiod::line_request> m_line_request_{};

            void SendStartSignal();

            int WaitForLow();

            /*
             * Wait for the GPIO pin to get high. Returns the microseconds waited until the pin git high.
             */
            int WaitForHigh();

            std::optional<dht11_data_t> ProcessData(uint64_t Data);


            uint8_t CalculateParity(uint8_t HumidityHigh, uint8_t HumidityLow,
                                    uint8_t TemperatureHigh, uint8_t TemperatureLow);

            DHT11_ERRORS getError();

        };
    }
}
#endif /* CDHT11_H_ */
