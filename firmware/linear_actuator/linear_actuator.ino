#include <TMC2209.h>
#include <Arduino.h>
#include <SerialUART.h>
#include <SerialUSB.h>

//initialize Serial 1
HardwareSerial & serial_stream = Serial1;

//initialize pins
#define SW_LIMIT 3
#define DIRECTION_PIN 20
#define STEP_PIN 26
#define SLEEP_PIN 27

//initialize direction
#define forward 0
#define backward 1

#define RUN_CURRENT_PERCENT 100
#define HALF_STEP_DELAY_MICROSECONDS 10

#define HALF_STEPS_FOR_FULL_REV 400

const unsigned int MAX_MESSAGE_LENGTH = 10;

// Instantiate TMC2209
TMC2209 stepper_driver;

void setup()
{
  Serial1.setTX(28);
  Serial1.setRX(29);
  
  Serial.begin(115200);
  Serial1.begin(115200);

  stepper_driver.setup(serial_stream);

  pinMode(SW_LIMIT, INPUT);
  pinMode(DIRECTION_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);

  stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
  stepper_driver.enableCoolStep();
  stepper_driver.enable();
}

void loop()
{
  while (Serial.available() == 0) {}
  String command = Serial.readString();
  command.trim();
  if (command.equals("home")) {
    Serial.println(command);
  }
  if (command.equals("magnet")) {
    Serial.println(command);
  }
  command = "";
}

//stepper driver commanded to move pos amount of revs
void move(double revs, int dir) {
  digitalWrite(DIRECTION_PIN, dir);
  for(int i; i<revs*HALF_STEPS_FOR_FULL_REV; i++) {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(HALF_STEP_DELAY_MICROSECONDS);
    if(digitalRead(SW_LIMIT) == 0) {
      break;
    }
  }
}

void magnet() {
  move(10, forward);
}

//resets the location at the lever and/or home location
void home() {
    move(10, backward);
}
