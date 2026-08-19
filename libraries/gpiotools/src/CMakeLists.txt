
find_path(GPIOD_INCLUDE_DIR
        NAMES gpiod.h
)

find_library(GPIOD_LIBRARY
        NAMES gpiod
)

add_library(pitools
        pitools.hpp
        gpio/gpiodevice.hpp
        gpio/GPIOInput.cpp
        gpio/GPIOInput.hpp
        gpio/gpiodevice.cpp
#        sensors/hc04.cpp
        sensors/dht11.cpp
#        sensors/hc04.hpp
        sensors/sensor.hpp

#        actuators/HC595.h
#        actuators/HC595.cpp
#        actuators/actuator.hpp
#        actuators/servo.cpp
#        actuators/servo.hpp
#        i2cdevices/LCD16x2.cpp
#        i2cdevices/LCD16x2.hpp
        defs.hpp
        defs.cpp
#        i2cdevices/I2CDevice.cpp
#        i2cdevices/I2CDevice.hpp
#        i2cdevices/mcp9808.cpp
#        i2cdevices/mcp9808.hpp
#        spidevices/SPIDevice.cpp
#        spidevices/SPIDevice.hpp
        gpio/GPIOOutput.cpp
        gpio/GPIOOutput.hpp
#        actuators/StepperMotor.cpp
#        actuators/StepperMotor.hpp
#        timers/WaitDuration.hpp
)

target_include_directories(pitools PRIVATE ${GPIOD_INCLUDE_DIR})
target_link_libraries(pitools PRIVATE ${GPIOD_LIBRARY})
