# PCA9624

Arduino library for [PCA9624](https://www.nxp.com/part/PCA9624PW#/) 8-bit Fm+ I2C-bus 100 mA 40 V LED driver

## Usage

```C++
#include <PCA9624.h>

static constexpr uint8_t PIN_LED_OE = 2;
PCA9624 pca9624 {0x60};
uint8_t v[8] {0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(PIN_LED_OE, OUTPUT);
    digitalWrite(PIN_LED_OE, HIGH);  // Output Disable

    pca9624.setup(Wire);

    delay(2000);

    digitalWrite(PIN_LED_OE, LOW);  // Output Enable
}

void loop() {
    for (size_t i = 0; i < 8; ++i) {
        v[i] += 1;
    }

    pca9624.drive(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
    delay(10);
}

```

## APIs

```C++
explicit PCA9624_(const uint8_t addr);
void setI2CAddress(const uint8_t addr);
void attach(WireType& w);
void setup(WireType& w);
void sleep(const bool b);
void setDriverOutputState(const LEDState state);
void drive(const uint8_t ch, const uint8_t vol);
void drive(const uint8_t vol);
void drive( const uint8_t v0, const uint8_t v1, const uint8_t v2, const uint8_t v3, const uint8_t v4, const uint8_t v5, const uint8_t v6, const uint8_t v7);
void verbose(const bool b);
```

```C++
enum class LEDState {
    DISABLE,            // LEDs are OFF
    FULLY_ON,           // uncontrollable, fully on
    PWM_EACH,           // controllable from PWMx
    PWM_EACH_AND_GROUP  // default: controllable from both PWMx and GRPPWM
};
```

## License

MIT
