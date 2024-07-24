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
#define DELAYVAL 500

//initialize direction
#define forward 1
#define backward 0

#define RUN_CURRENT_PERCENT 100
#define STEPS_FOR_FULL_REV 51200
#define RUN_VELOCITY 40000

#define HALF_STEP_DELAY_MICROSECONDS 20

#define OPERATING_LENGTH 80

const unsigned int MAX_MESSAGE_LENGTH = 24;
unsigned int CURRENT_POS = 0;
const int TOTAL_POS = OPERATING_LENGTH*51200*2/5;

// Instantiate TMC2209
TMC2209 stepper_driver;

void setup()
{
  #if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  #endif

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

  pixels.begin();
  pixels.setBrightness(20);
  pixels.fill(0xFF0000);
  pixels.show();

  CURRENT_POS = 0;
}

void loop()
{
  digitalWrite(ENABLE_PIN, HIGH);
  pixels.show();
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Read the command until newline
    input.trim(); // Remove any leading/trailing whitespace

    // Separate command and optional number
    String command;
    float number = 0.0;

    int spaceIndex = input.indexOf(' '); // Find the space

    if (spaceIndex == -1) {
      // No space found, whole input is command
      command = input;
    }
    else {
      // Extract command and number
      command = input.substring(0, spaceIndex); // Extract command part
      String numberStr = input.substring(spaceIndex + 1); // Extract number part
      number = numberStr.toFloat(); // Convert number string to float
    }

    if (command.equals("home")) {
      returnHome();
    }
    if (command.equals("magnet")) {
      magnet();
    }
    if (command.equals("forward")) {
      Serial.println("manual direction set to forward");
      digitalWrite(DIRECTION_PIN, 1);
    }
    if (command.equals("backward")) {
      Serial.println("manual direction set to backward");
      digitalWrite(DIRECTION_PIN, 0);
    }
    if (number != 0) {
      Serial.print("moving ");
      Serial.print(number);
      Serial.println(" millimeter(s)");
      manualMove(number);
    }
    else {
      Serial.println("didn't recognize command");
    }
  }
}

void manualMove(double number) {
  pixels.fill(0xFFFF00);
  pixels.show();
  digitalWrite(ENABLE_PIN, LOW);
  for(int i = 0; i<(number*STEPS_FOR_FULL_REV*2/5); i++) {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(HALF_STEP_DELAY_MICROSECONDS);
    if (digitalRead(DIRECTION_PIN) == 1) {
      CURRENT_POS++;
    }
    if (digitalRead(DIRECTION_PIN) == 0) {
      CURRENT_POS--;
    }
    if(lengthCheck() == true || safetyStop() == true) {
      Serial.println("emergency stopped successfully");
      pixels.fill(0xFF0000);
      pixels.show();
      break;
    }
  }
}

//stepper driver commanded to move pos amount of revs with direction considered
void move(double number, int dir) {
  pixels.fill(0xFFFF00);
  pixels.show();
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIRECTION_PIN, dir);
  for(int i = 0; i<(number*STEPS_FOR_FULL_REV*2/5); i++) {
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(HALF_STEP_DELAY_MICROSECONDS);
    if (digitalRead(DIRECTION_PIN) == 1) {
      CURRENT_POS++;
    }
    if (digitalRead(DIRECTION_PIN) == 0) {
      CURRENT_POS--;
    }
    if(lengthCheck() == true || safetyStop() == true) {
      Serial.println("emergency stopped successfully");
      pixels.fill(0xFF0000);
      pixels.show();
      break;
    }
  }
}

//resets to home location
void returnHome() {
  pixels.fill(0xFFFF00);
  pixels.show();
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIRECTION_PIN, LOW);
  Serial.println("heading home");
  stepper_driver.moveAtVelocity(RUN_VELOCITY);
  while (digitalRead(SW_LIMIT) == HIGH) {}
  stepper_driver.moveAtVelocity(0);
  CURRENT_POS = 0;
  pixels.fill(0x00FF00);
  pixels.show();
  Serial.println("returned home");
  // safetyStop();
}

//moves the EPR into the magnet
void magnet() {
  digitalWrite(ENABLE_PIN, LOW);
  Serial.println("approaching magnet");
  move(50, forward);
  Serial.println("reached magnet");
  pixels.fill(0x0000FF);
  pixels.show();
  if(lengthCheck() == true || safetyStop() == true) {
    pixels.fill(0xFF0000);
    pixels.show();
  }
}

bool lengthCheck() {
  if (CURRENT_POS> TOTAL_POS || CURRENT_POS < 0) {
    pixels.fill(255, 0, 0);
    pixels.show();
    return true;
  }
  return false;
}

bool safetyStop() {
  if (Serial.available() == 1) {
    String stop = Serial.readStringUntil('\n');
    stop.trim();
    if (stop.equals("stop")) {
      return true;
    }
  }
  return false;
}