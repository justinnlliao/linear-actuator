#include <TMC2209.h>
#include <Arduino.h>
#include <SerialUART.h>
#include <SerialUSB.h>
#include <Adafruit_NeoPixel.h>

//initialize Serial 1
SerialUART & serial_stream = Serial1;

//initialize pins
#define SW_LIMIT 3
#define DIRECTION_PIN 20
#define STEP_PIN 26
#define SLEEP_PIN 27
#define ENABLE_PIN 4

#define NUMPIXELS 3

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

//initialize direction
#define forward 0
#define backward 1

#define RUN_CURRENT_PERCENT 100
<<<<<<< Updated upstream
#define HALF_STEP_DELAY_MICROSECONDS 8
#define STEPS_FOR_FULL_REV 51200
=======
#define HALF_STEP_DELAY_MICROSECONDS 10

#define HALF_STEPS_FOR_FULL_REV 51200*2
>>>>>>> Stashed changes

const unsigned int MAX_MESSAGE_LENGTH = 10;

// Instantiate TMC2209
TMC2209 stepper_driver;

void setup()
{
  stepper_driver.setup(serial_stream, 115200, TMC2209::SERIAL_ADDRESS_0, 29, 28);
  
  Serial.begin(115200);
  Serial1.begin(115200);

  pinMode(SW_LIMIT, INPUT);
  pinMode(DIRECTION_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  stepper_driver.setup(serial_stream);
  digitalWrite(ENABLE_PIN, LOW);
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
<<<<<<< Updated upstream
    Serial.println(command);
=======
    digitalWrite(25, HIGH);

>>>>>>> Stashed changes
    home();
  }
  if (command.equals("magnet")) {
    Serial.println(command);

    home();
  }
}

//stepper driver commanded to move pos amount of revs
void move(double revs, int dir) {
  digitalWrite(DIRECTION_PIN, dir);
  for(int i; i<revs*STEPS_FOR_FULL_REV; i++) {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(HALF_STEP_DELAY_MICROSECONDS);
    if(digitalRead(SW_LIMIT) == HIGH) {
      break;
    }
  }
}

//resets the location at the lever and/or home location
void home() {
  Serial1.println("returning home");
  move(10, backward);
}

//moves the EPR into the magnet
void magnet() {
  Serial1.println("approaching magnet");
  move(10, forward);
}