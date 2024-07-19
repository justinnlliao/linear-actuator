#include <TMC2209.h>
#include <Arduino.h>
#include <SerialUART.h>
#include <SerialUSB.h>
#include <Adafruit_NeoPixel.h>
#include <ctype.h>

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

#define OPERATING_LENGTH 83

#define RUN_CURRENT_PERCENT 100
#define HALF_STEP_DELAY_MICROSECONDS 8
#define STEPS_FOR_FULL_REV 51200
#define RUN_VELOCITY 20000

#define HALF_STEP_DELAY_MICROSECONDS 40

const unsigned int MAX_MESSAGE_LENGTH = 24;
const unsigned int CURRENT_POS = 0;
// const unsigned int time = 0;

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
  digitalWrite(ENABLE_PIN, HIGH);
  stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
  stepper_driver.enableCoolStep();
  stepper_driver.enable();
}

void loop()
{
  while (Serial.available() == 1) {
    digitalWrite(ENABLE_PIN, HIGH);
    String command = Serial.readString();
    int millis = Serial.parseInt();
    command.trim();
    if (command.equals("home")) {
      returnHome();
    }
    if (command.equals("magnet")) {
      magnet();
    }
    if (command.equals("forward")) {
      Serial1.println("manual direction set to forward");
      digitalWrite(DIRECTION_PIN, 0);
    }
    if (command.equals("backward")) {
      Serial1.println("manual direction set to backward");
      digitalWrite(DIRECTION_PIN, 1);
    }
    if (millis != 0) {
      int millis = command.toInt();
      Serial1.print("moving ");
      Serial1.print(millis);
      Serial1.println(" millimeter(s)");
      move(millis);
    }
    else {
      Serial1.println("didn't recognize command");
    }

  }
  digitalWrite(ENABLE_PIN, HIGH);
  Serial1.println("no USB connection");
}

void manualMove(double millis) {
  for(int i; i<(millis*STEPS_FOR_FULL_REV*2/5); i++) {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(HALF_STEP_DELAY_MICROSECONDS);
    if(digitalRead(SW_LIMIT) == HIGH) {
      break;
    }
    // safetyStop();
  }
}

void move(double millis) {
  digitalWrite(ENABLE_PIN, LOW);
  for(int i; i<(millis*STEPS_FOR_FULL_REV*2/5); i++) {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(HALF_STEP_DELAY_MICROSECONDS);
    if(digitalRead(SW_LIMIT) == HIGH) {
      break;
    }
    // safetyStop();
  }
}

//stepper driver commanded to move pos amount of revs with direction considered
void move(double millis, int dir) {
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIRECTION_PIN, dir);
  for(int i; i<(millis*STEPS_FOR_FULL_REV*2/5); i++) {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(HALF_STEP_DELAY_MICROSECONDS);
    if(digitalRead(SW_LIMIT) == HIGH) {
      break;
    }
    // safetyStop();
  }
}

//resets to home location
void returnHome() {
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIRECTION_PIN, LOW);
  stepper_driver.moveAtVelocity(RUN_VELOCITY);
  if (digitalRead(SW_LIMIT) == 1) {
    stepper_driver.moveAtVelocity(0);
    Serial1.println("returned home");
  }
  // safetyStop();
}

//moves the EPR into the magnet
void magnet() {
  digitalWrite(ENABLE_PIN, LOW);
  Serial1.println("approaching magnet");
  move(50, forward);
}

// void safetyStop() {
//   if (Serial.available() == 1) {
//   String stop = Serial.read();
//     if (stop.equals("stop")) {
//       stepper_driver.moveAtVelocity(0);
//       Serial1.println("stopped successfully")
//       break();
//     }
//   }
// }