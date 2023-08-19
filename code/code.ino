// **************************************************************************************************
// Constants
const int LEFT_MOTOR_PIN_A = 5;
const int LEFT_MOTOR_PIN_B = 6;
const int RIGHT_MOTOR_PIN_A = 9;
const int RIGHT_MOTOR_PIN_B = 10;
const int ULTRASONIC_TRIGGER_PIN = 7;
const int ULTRASONIC_ECHO_PIN = 8;
// Variables
float duration_us, distance_cm;
unsigned long startMillis;
unsigned long currentMillis;
// **************************************************************************************************
const int FORWARD_SPEED = 255;           // Set the value between 0 and 255
const int BACKWARD_SPEED = 255;          // Set the value between 0 and 255
const int DISTANCE_LOWER_BOUNDARY = 4;   // Set the lower boundary between the drone and the obstacle
const int DISTANCE_UPPER_BOUNDARY = 20;  // Set the upper boundary between the drone and the obstacle

void setup() {
  // This part is for setting up the Arduino pins and serial connection, please keep it unchanged
  pinMode(LEFT_MOTOR_PIN_A, OUTPUT);
  pinMode(LEFT_MOTOR_PIN_B, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_A, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_B, OUTPUT);
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  Serial.begin(9600);
  startMillis = millis();
}

// ******************************************************************************************************
void loop() {
  // distance_measurement();
  // distance_maintaining_logic();
  forward();
  delay(2000);
  backward();
  delay(2000);
  left_spin();
  delay(2000);
  right_spin();
  delay(2000);
}
// ******************************************************************************************************

// Function for the movements
// ******************************************************************************************************
void forward() {
  // Function that moves the robot forward
  analogWrite(LEFT_MOTOR_PIN_A, FORWARD_SPEED);
  digitalWrite(LEFT_MOTOR_PIN_B, LOW);
  analogWrite(RIGHT_MOTOR_PIN_A, FORWARD_SPEED);
  digitalWrite(RIGHT_MOTOR_PIN_B, LOW);
}

// ******************************************************************************************************
void backward() {
  // Function that moves the robot backward
  digitalWrite(LEFT_MOTOR_PIN_A, LOW);
  analogWrite(LEFT_MOTOR_PIN_B, BACKWARD_SPEED);
  digitalWrite(RIGHT_MOTOR_PIN_A, LOW);
  analogWrite(RIGHT_MOTOR_PIN_B, BACKWARD_SPEED);
}

// ******************************************************************************************************
void left_spin() {
  // Function that makes the robot spin to the left direction
  digitalWrite(LEFT_MOTOR_PIN_A, LOW);
  analogWrite(LEFT_MOTOR_PIN_B, FORWARD_SPEED);
  analogWrite(RIGHT_MOTOR_PIN_A, FORWARD_SPEED);
  digitalWrite(RIGHT_MOTOR_PIN_B, LOW);
}

// ******************************************************************************************************
void right_spin() {
  // Function that makes the robot spin to the right direction
  analogWrite(LEFT_MOTOR_PIN_A, FORWARD_SPEED);
  digitalWrite(LEFT_MOTOR_PIN_B, LOW);
  digitalWrite(RIGHT_MOTOR_PIN_A, LOW);
  analogWrite(RIGHT_MOTOR_PIN_B, FORWARD_SPEED);
}

void STOP() {
  digitalWrite(LEFT_MOTOR_PIN_A, LOW);
  digitalWrite(LEFT_MOTOR_PIN_B, LOW);
  digitalWrite(RIGHT_MOTOR_PIN_A, LOW);
  digitalWrite(RIGHT_MOTOR_PIN_B, LOW);
}
// ***** Missing left and right turn by setting differential speed ***** //

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

// Functions that controls the movement of the robot drone
// ******************************************************************************************************
void distance_maintaining_logic() {
  if (distance_cm < 5) {
    backward();
    Serial.println(">>>Going back");
  } else if (distance_cm > 7) {
    forward();
    Serial.println(">>>Going forward");
  } else {
    STOP();
  }
}

