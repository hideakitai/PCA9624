#pragma once
#ifndef ARDUINO_PCA9624_H
#define ARDUINO_PCA9624_H

#include <Arduino.h>
#include <Wire.h>

template <typename WireType>
class PCA9624_ {
    enum class Reg {
        MODE1,
        MODE2,
        PWM0,
        PWM1,
        PWM2,
        PWM3,
        PWM4,
        PWM5,
        PWM6,
        PWM7,
        GRPPWM,
        GRPFREQ,
        LEDOUT0,
        LEDOUT1,
        SUBADR1,
        SUBADR2,
        SUBADR3,
        ALLCALLADR
    };

    uint8_t i2c_addr {0};
    uint8_t i2c_err {0};
    bool b_verbose {false};
    uint8_t leds[8];
    WireType* wire {nullptr};

public:
    enum class LEDState {
        DISABLE,            // LEDs are OFF
        FULLY_ON,           // uncontrollable, fully on
        PWM_EACH,           // controllable from PWMx
        PWM_EACH_AND_GROUP  // controllable from both PWMx and GRPPWM
    };

    explicit PCA9624_(const uint8_t addr) : i2c_addr(addr), leds() {}

    void setI2CAddress(const uint8_t addr) {
        i2c_addr = addr;
    }

    void attach(WireType& w) {
        wire = &w;
    }

    void setup(WireType& w) {
        attach(w);
        sleep(false);
        setDriverOutputState(LEDState::PWM_EACH_AND_GROUP);
    }

    void sleep(const bool b) {
        uint8_t data = 0;
        if (b)
            data = 0b10010001;
        else
            data = 0b10000001;
        writeByte(Reg::MODE1, data);
    }

    void setDriverOutputState(const LEDState state) {
        uint8_t data[2];
        data[0] = ((uint8_t)state << 6) | ((uint8_t)state << 4) | ((uint8_t)state << 2) | ((uint8_t)state << 0);
        data[1] = ((uint8_t)state << 6) | ((uint8_t)state << 4) | ((uint8_t)state << 2) | ((uint8_t)state << 0);
        writeBytes(Reg::LEDOUT0, data, sizeof(data));
    }

    void setGroupPWM(const uint8_t vol) {
        writeByte(Reg::GRPPWM, vol);
    }

    void drive(const uint8_t ch, const uint8_t vol) {
        writeByte(i2c_addr, (uint8_t)Reg::PWM0 + ch, vol);
    }

    void drive(const uint8_t vol) {
        uint8_t data[8];
        for (uint8_t i = 0; i < 8; ++i) data[i] = vol;
        writeBytes(Reg::PWM0, data, 8);
    }

    void drive(
        const uint8_t v0,
        const uint8_t v1,
        const uint8_t v2,
        const uint8_t v3,
        const uint8_t v4,
        const uint8_t v5,
        const uint8_t v6,
        const uint8_t v7) {
        uint8_t data[8] {v0, v1, v2, v3, v4, v5, v6, v7};
        writeBytes(Reg::PWM0, data, 8);
    }

    void verbose(const bool b) {
        b_verbose = b;
    }

private:
    void writeByte(const Reg reg, const uint8_t data) {
        writeByte(i2c_addr, (uint8_t)reg, data);
    }

    void writeByte(const uint8_t address, const uint8_t subAddress, const uint8_t data) {
        wire->beginTransmission(address);   // Initialize the Tx buffer
        wire->write(subAddress);            // Put slave register address in Tx buffer
        wire->write(data);                  // Put data in Tx buffer
        i2c_err = wire->endTransmission();  // Send the Tx buffer
        if (i2c_err) pirntI2CError();
    }

    void writeBytes(const Reg reg, const uint8_t* data, const uint8_t size) {
        writeBytes(i2c_addr, (uint8_t)reg, data, size);
    }

    void writeBytes(const uint8_t address, const uint8_t subAddress, const uint8_t* data, const uint8_t size) {
        wire->beginTransmission(address);                             // Initialize the Tx buffer
        wire->write(subAddress | 0x80);                               // auto increment
        for (uint8_t i = 0; i < size; ++i) wire->write(*(data + i));  // Put data in Tx buffer
        i2c_err = wire->endTransmission();                            // Send the Tx buffer
        if (i2c_err) pirntI2CError();
    }

    uint8_t readByte(const uint8_t address, const uint8_t subAddress) {
        uint8_t data = 0;                        // `data` will store the register data
        wire->beginTransmission(address);        // Initialize the Tx buffer
        wire->write(subAddress);                 // Put slave register address in Tx buffer
        i2c_err = wire->endTransmission(false);  // Send the Tx buffer, but send a restart to keep connection alive
        if (i2c_err) pirntI2CError();
        wire->requestFrom(address, (size_t)1);       // Read one byte from slave register address
        if (wire->available()) data = wire->read();  // Fill Rx buffer with result
        return data;                                 // Return data read from slave register
    }

    void readBytes(const uint8_t address, const uint8_t subAddress, const uint8_t count, const uint8_t* dest) {
        wire->beginTransmission(address);        // Initialize the Tx buffer
        wire->write(subAddress);                 // Put slave register address in Tx buffer
        i2c_err = wire->endTransmission(false);  // Send the Tx buffer, but send a restart to keep connection alive
        if (i2c_err) pirntI2CError();
        uint8_t i = 0;
        wire->requestFrom(address, count);  // Read bytes from slave register address
        while (wire->available()) {
            dest[i++] = wire->read();
        }  // Put read results in the Rx buffer
    }

    void pirntI2CError() {
        if (i2c_err == 7) return;  // to avoid stickbreaker-i2c branch's error code
        if (b_verbose) {
            Serial.print("I2C ERROR CODE : ");
            Serial.println(i2c_err);
        }
    }
};

using PCA9624 = PCA9624_<TwoWire>;

#endif  // ARDUINO_PCA9624_H
