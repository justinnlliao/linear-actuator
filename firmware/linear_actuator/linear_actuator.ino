#include <TMC2209.h>

HardwareSerial & serial_stream = Serial1;

const long SERIAL_BAUD_RATE = 115200;
const int DELAY = 2000;
const int32_t VELOCITY = 20000;
// current values may need to be reduced to prevent overheating depending on
// specific motor and power supply voltage
const uint8_t RUN_CURRENT_PERCENT = 100;

// Instantiate TMC2209
TMC2209 stepper_driver;

void setup() {
  // reassign pin numbers for Serial1
  Serial1.setTX(28);
  Serial1.setRX(29);
  // initialize both serial ports:
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop() {
  Serial.println("skibidi");
}