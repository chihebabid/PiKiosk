// Compatibility shim to replace a subset of pigpio API with libgpiod
// Provides: gpioSetMode, gpioRead, gpioWrite, gpioSetPullUpDown, gpioTick
// Simple implementation: opens /dev/gpiochip0 and requests lines on demand.
#pragma once

#include <gpiod.h>
#include <unordered_map>
#include <mutex>
#include <cstdint>
#include <chrono>
#include <time.h>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

namespace {
    struct LineInfo {
        gpiod_line *line{nullptr};
        bool is_output{false};
    };

    static std::mutex g_lines_mutex;
    static std::unordered_map<int, LineInfo> g_lines;
    static gpiod_chip *g_chip = nullptr;

    static void ensure_chip_open() {
        if (g_chip) return;
        g_chip = gpiod_chip_open_by_name("gpiochip0");
        if (!g_chip) {
            throw std::runtime_error("failed to open /dev/gpiochip0");
        }
    }
}

// pigpio compatible enums used in the codebase
#define PI_OUTPUT 1
#define PI_INPUT 0
#define PI_PUD_OFF 0
#define PI_PUD_DOWN 1
#define PI_PUD_UP 2

inline void gpioSetMode(unsigned pin, int mode) {
    std::lock_guard<std::mutex> lock(g_lines_mutex);
    ensure_chip_open();
    auto it = g_lines.find(pin);
    if (it != g_lines.end()) {
        // if mode unchanged, keep
        if (it->second.is_output == (mode==PI_OUTPUT)) return;
        // release existing line
        if (it->second.line) gpiod_line_release(it->second.line);
        g_lines.erase(it);
    }
    gpiod_line *line = gpiod_chip_get_line(g_chip, pin);
    if (!line) throw std::runtime_error("failed to get line");
    int ret;
    if (mode==PI_OUTPUT) {
        ret = gpiod_line_request_output(line, "pitools", 0);
        if (ret<0) throw std::runtime_error("failed to request line as output");
        g_lines[pin] = {line, true};
    } else {
        ret = gpiod_line_request_input(line, "pitools");
        if (ret<0) throw std::runtime_error("failed to request line as input");
        g_lines[pin] = {line, false};
    }
}

inline int gpioRead(unsigned pin) {
    std::lock_guard<std::mutex> lock(g_lines_mutex);
    ensure_chip_open();
    auto it = g_lines.find(pin);
    if (it == g_lines.end()) {
        // try to request as input implicitly
        gpiod_line *line = gpiod_chip_get_line(g_chip, pin);
        if (!line) throw std::runtime_error("failed to get line for read");
        if (gpiod_line_request_input(line, "pitools")<0) throw std::runtime_error("failed to request input for read");
        g_lines[pin] = {line,false};
        it = g_lines.find(pin);
    }
    int v = gpiod_line_get_value(it->second.line);
    if (v<0) throw std::runtime_error("failed to read line value");
    return v;
}

inline void gpioWrite(unsigned pin, int value) {
    std::lock_guard<std::mutex> lock(g_lines_mutex);
    ensure_chip_open();
    auto it = g_lines.find(pin);
    if (it == g_lines.end()) {
        // implicitly request as output
        gpiod_line *line = gpiod_chip_get_line(g_chip, pin);
        if (!line) throw std::runtime_error("failed to get line for write");
        if (gpiod_line_request_output(line, "pitools", value)<0) throw std::runtime_error("failed to request output for write");
        g_lines[pin] = {line,true};
        return;
    }
    if (!it->second.is_output) {
        // re-request as output
        gpiod_line_release(it->second.line);
        gpiod_line *line = gpiod_chip_get_line(g_chip, pin);
        if (!line) throw std::runtime_error("failed to get line for write 2");
        if (gpiod_line_request_output(line, "pitools", value)<0) throw std::runtime_error("failed to request output for write2");
        it->second = {line,true};
        return;
    }
    if (gpiod_line_set_value(it->second.line, value)<0) throw std::runtime_error("failed to set line value");
}

// pull-up/down: libgpiod historically doesn't provide changing bias; noop
inline void gpioSetPullUpDown(unsigned pin, int pud) {
    (void)pin; (void)pud; // no-op: configure in device tree / boot overlay if needed
}

// microsecond ticks
inline uint32_t gpioTick() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t us = uint64_t(ts.tv_sec) * 1000000ULL + uint64_t(ts.tv_nsec) / 1000ULL;
    return static_cast<uint32_t>(us & 0xFFFFFFFF);
}

// --- I2C compatibility wrappers (simple i2c-dev based) ---
// Note: pigpio i2cOpen signature is (unsigned bus, unsigned addr, unsigned flags)
inline int i2cOpen(unsigned addr, unsigned bus, unsigned flags) {
    (void)flags;
    char path[32];
    snprintf(path, sizeof(path), "/dev/i2c-%u", bus);
    int fd = open(path, O_RDWR);
    if (fd < 0) return -1;
    if (ioctl(fd, I2C_SLAVE, addr) < 0) { close(fd); return -1; }
    return fd;
}

inline void i2cClose(int handle) {
    if (handle>=0) close(handle);
}

inline int i2cWriteByte(int handle, uint8_t b) {
    uint8_t buf[1]{b};
    ssize_t r = write(handle, buf, 1);
    return (r==1)?0:-1;
}

inline int i2cWriteWordData(int handle, uint8_t reg, uint16_t data) {
    uint8_t buf[3];
    buf[0]=reg;
    buf[1]=data & 0xFF; // LSB
    buf[2]=(data>>8) & 0xFF; // MSB
    ssize_t r = write(handle, buf, 3);
    return (r==3)?0:-1;
}

inline int i2cReadWordData(int handle, uint8_t reg) {
    uint8_t buf[2];
    // write register
    if (write(handle, &reg, 1)!=1) return -1;
    // read two bytes
    ssize_t r = read(handle, buf, 2);
    if (r!=2) return -1;
    // return 16-bit value LSB first
    return (int)((buf[1]<<8) | buf[0]);
}

// --- SPI compatibility wrappers (spidev) ---
inline int spiOpen(unsigned channel, unsigned speed, unsigned flags) {
    // channel -> usually 0 means /dev/spidev0.0, 1 -> /dev/spidev0.1
    char path[32];
    snprintf(path, sizeof(path), "/dev/spidev0.%u", channel);
    int fd = open(path, O_RDWR);
    if (fd < 0) return -1;
    uint8_t mode = flags & 0x3; // pigpio encodes mode low bits
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) { close(fd); return -1; }
    uint32_t speed32 = speed;
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed32) < 0) { close(fd); return -1; }
    return fd;
}

inline void spiClose(int h) {
    if (h>=0) close(h);
}

// --- Servo / PWM stub ---
// pigpio offers gpioServo(pin, pulsewidth). Implemented here as noop; recommend using kernel PWM for stable servo control.
inline void gpioServo(unsigned pin, unsigned pulsewidth) {
    (void)pin; (void)pulsewidth;
    // no-op: not implemented in libgpiod shim. Use kernel PWM or a dedicated PWM library for servo control.
}


