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
