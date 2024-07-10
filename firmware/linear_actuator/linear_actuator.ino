#include <TMC2209.h>
#include <Arduino.h>

//initialize Serial 1
HardwareSerial & serial_stream = Serial1;

//initialize pins
#define STEP_PIN 26
#define DIRECTION_PIN 20
#define SW_LIMIT 3 

//initialize direction
#define forward 0
#define backward 1

#define RUN_CURRENT_PERCENT 100

const int32_t STEP_COUNT = 51200;
const uint16_t HALF_STEP_DURATION_MICROSECONDS = 10;

// Instantiate TMC2209
TMC2209 stepper_driver;

void setup()
{
  Serial1.setTX(28);
  Serial1.setRX(29);
  
  stepper_driver.setup(serial_stream);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIRECTION_PIN, OUTPUT);
  pinMode()

  stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
  stepper_driver.enableCoolStep();
  stepper_driver.enable();
}

void loop()
{
  // One step takes two iterations through the for loop
  for (int i=0; i<STEP_COUNT*2; ++i)
  {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(HALF_STEP_DURATION_MICROSECONDS);
  }
  digitalWrite(DIRECTION_PIN, !digitalRead(DIRECTION_PIN));
  delay(STOP_DURATION);
}

//resets the location at the lever and/or home location
void home() {
  while (digitalRead(SW_LIMIT) == 1) {
    move(backward);
  }
}

void move(int dir) {
  digitalWrite(DIRECTION_PIN, dir);
  digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
  delayMicroseconds(HALF_STEP_DURATION_MICROSECONDS);
}

//stepper driver commanded to move pos amount of revs
void move(double pos, int dir) {
  digitialWrite(DIRECTION_PIN, dir);
  digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
  delayMicroseconds(HALF_STEP_DURATION_MICROSECONDS);
}