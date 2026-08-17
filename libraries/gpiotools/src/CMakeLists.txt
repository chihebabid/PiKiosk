
find_package(PkgConfig REQUIRED)

pkg_check_modules(GPIODCXX REQUIRED libgpiodcxx)
pkg_check_modules(LIBGPIOD REQUIRED libgpiod)




add_library(pitools pitools.hpp
        gpio/gpiodevice.hpp
        gpio/GPIOInput.cpp
        gpio/GPIOInput.hpp
        gpio/gpiodevice.cpp
        sensors/hc04.cpp sensors/dht11.cpp
        sensors/hc04.hpp sensors/dht11.h
        sensors/sensor.hpp
        sensors/dht11.h
        actuators/HC595.h
        actuators/HC595.cpp
        actuators/actuator.hpp
        actuators/servo.cpp
        actuators/servo.hpp
        i2cdevices/LCD16x2.cpp
        i2cdevices/LCD16x2.hpp
        defs.hpp
        defs.cpp
        i2cdevices/I2CDevice.cpp
        i2cdevices/I2CDevice.hpp
        i2cdevices/mcp9808.cpp
        i2cdevices/mcp9808.hpp
        spidevices/SPIDevice.cpp
        spidevices/SPIDevice.hpp
        gpio/GPIOOutput.cpp
        gpio/GPIOOutput.hpp
        actuators/StepperMotor.cpp
        actuators/StepperMotor.hpp
        timers/WaitDuration.hpp)

target_include_directories(pitools PRIVATE ${GPIODCXX_INCLUDE_DIRS} ${LIBGPIOD_INCLUDE_DIRS})
target_link_directories(pitools PRIVATE ${GPIODCXX_LIBRARY_DIRS} ${LIBGPIOD_LIBRARY_DIRS})
target_link_libraries(pitools PRIVATE ${GPIODCXX_LIBRARIES} ${LIBGPIOD_LIBRARIES})
