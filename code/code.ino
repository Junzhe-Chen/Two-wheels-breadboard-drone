// Libraries
// **************************************************************************************************
#include <Servo.h>
Servo leftServo, rightServo;
// **************************************************************************************************
// Constants
const int ULTRASONIC_TRIGGER_PIN = 7;
const int ULTRASONIC_ECHO_PIN = 8;
const int LEFT_SERVO_PIN = 5;
const int LEFT_SIGNAL_LIGHT = 6;
const int RIGHT_SERVO_PIN = 10;
const int RIGHT_SIGNAL_LIGHT = 9;
const int FRONT_LED = 11;
const int LIGHT_SENSOR_PIN = A5;
// Variables
float duration_us, distance_cm;
int light_intensity;
int leftServoSpeed, rightServoSpeed = 90;
// According to the datasheet, when inputting 90 the servo should stop. If the servo does not stop
// at 90, try to calibrate the potentiometer until the servo stop
unsigned long startMillis;
unsigned long currentMillis;
// **************************************************************************************************
// User determined values
const int FORWARD_SPEED = 10;            // Set the value between 0 and 90
const int BACKWARD_SPEED = 10;           // Set the value between 0 and 90
const int DISTANCE_LOWER_BOUNDARY = 15;  // Set the lower boundary between the drone and the obstacle
const int DISTANCE_UPPER_BOUNDARY = 20;  // Set the upper boundary between the drone and the obstacle
const int OFFSET_ANGLE = 200;


void setup() {
  // This part is for setting up the Arduino pins and serial connection, please keep it unchanged
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(LEFT_SIGNAL_LIGHT, OUTPUT);
  pinMode(RIGHT_SIGNAL_LIGHT, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(FRONT_LED, OUTPUT);
  leftServo.attach(5);
  rightServo.attach(10);
  Serial.begin(9600);
  startMillis = millis();
}

// ******************************************************************************************************
void loop() {

  obstacle_avoidance_logic();
  light_sensing();
  // forward();
}
// ******************************************************************************************************




// Functions for the movement (servo)
// ******************************************************************************************************
void forward() {
  // leftServo.write(90 - FORWARD_SPEED);
  // rightServo.write(90 + FORWARD_SPEED);
  leftServo.write(83);
  rightServo.write(100);
  digitalWrite(LEFT_SIGNAL_LIGHT, LOW);
  digitalWrite(RIGHT_SIGNAL_LIGHT, LOW);
}

void backward() {
  // leftServo.write(90 + FORWARD_SPEED);
  // rightServo.write(90 - FORWARD_SPEED);
  leftServo.write(100);
  rightServo.write(83);
  digitalWrite(LEFT_SIGNAL_LIGHT, HIGH);
  digitalWrite(RIGHT_SIGNAL_LIGHT, HIGH);
}

void spin_left() {
  leftServo.write(90 - FORWARD_SPEED);
  rightServo.write(90 - FORWARD_SPEED);
  digitalWrite(LEFT_SIGNAL_LIGHT, HIGH);
  digitalWrite(RIGHT_SIGNAL_LIGHT, LOW);
}

void spin_right() {
  leftServo.write(90 + FORWARD_SPEED);
  rightServo.write(90 + FORWARD_SPEED);
  digitalWrite(LEFT_SIGNAL_LIGHT, LOW);
  digitalWrite(RIGHT_SIGNAL_LIGHT, HIGH);
}

void STOP() {
  leftServo.write(90);
  rightServo.write(90);
  digitalWrite(LEFT_SIGNAL_LIGHT, HIGH);
  digitalWrite(RIGHT_SIGNAL_LIGHT, HIGH);
}




// ******************************************************************************************************
void distance_measurement() {
  if (millis() > currentMillis + 10) {  // Distance measurement in every 100 milliseconds
    currentMillis = millis();
    // generate 10-microsecond pulse to TRIG pin
    digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);

    // measure duration of pulse from ECHO pin
    duration_us = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);

    // calculate the distance
    distance_cm = 0.017 * duration_us;

    // print the value to Serial Monitor
    Serial.print("distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
  }
}

int checkLeft() {
  STOP();
  delay(10);
  spin_left();
  delay(200);  // Calibrate this value till when doing one spin, the robot turns to 45 degrees
  distance_measurement();
  spin_right();
  delay(200);  // Spin the robot back after checking left distance
  STOP();
  Serial.print(" >>> Distance to the left is: ");
  Serial.print(distance_cm);
  Serial.println("cm <<<");
  return distance_cm;
}

int checkRight() {
  STOP();
  delay(10);
  spin_right();
  delay(200);  // Calibrate this value till when doing one spin, the robot turns to 45 degrees
  STOP();
  distance_measurement();
  Serial.print(" >>> Distance to the right is: ");
  Serial.print(distance_cm);
  Serial.println("cm <<<");
  return distance_cm;
}

// ******************************************************************************************************
void obstacle_avoidance_logic() {
  int distanceLeft, distanceRight;
  distance_measurement();
  if (distance_cm <= DISTANCE_LOWER_BOUNDARY) {

    backward();  // When the distance is lower than the threshold, going back
    delay(200);

    // Measure the left and right distance between the drone and the obstacle, go through the side with lower distance
    checkLeft();
    checkRight();
    distanceLeft = checkLeft();
    distanceRight = checkRight();
    STOP();
    delay(200);

    if (distanceLeft <= distanceRight) {
      // When left distance is less than right distance, that means there is a gap at left, turn left and go straight
      spin_right();
      delay(OFFSET_ANGLE);
      STOP();
      delay(100);
      forward();
    } else {
      // When left distance is more than right distance, that means there is a gap at right, turn right and go straight
      spin_left();
      delay(OFFSET_ANGLE);
      STOP();
      delay(100);
      forward();
    }
  } else {
    forward();  // When the distance is higher than the threshold, going forward
  }
  delay(100);
}

// ******************************************************************************************************
void light_sensing() {
  Serial.println("light sensing debug");
  if (millis() > currentMillis + 100) {
    currentMillis = millis();
    light_intensity = analogRead(LIGHT_SENSOR_PIN);
    Serial.println(light_intensity);
    if (light_intensity < 250) {
      digitalWrite(FRONT_LED, HIGH);
      Serial.println("Turn on the front light");
    } else {
      digitalWrite(FRONT_LED, LOW);
    }
  }
}

//// Functions that controls the movement of the robot drone
//// ******************************************************************************************************
void distance_maintaining_logic() {
  if (distance_cm < 8) {
    backward();
    Serial.println(">>>Going back");
  } else if (distance_cm > 12) {
    forward();
    Serial.println(">>>Going forward");
  } else {
    STOP();
  }
}
