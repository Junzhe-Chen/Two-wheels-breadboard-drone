// **************************************************************************************************
const int LEFT_MOTOR_PIN1 = 5;
const int LEFT_MOTOR_PIN2 = 6;
const int RIGHT_MOTOR_PIN1 = 9;
const int RIGHT_MOTOR_PIN2 = 10  ;
const int ULTRASONIC_ECHO_PIN = 8;
const int ULTRASONIC_TRIGGER_PIN = 7;
// Values that will be used in the program
float duration_us, distance_cm;
unsigned long startMillis;
unsigned long currentMillis;
// **************************************************************************************************
const int FORWARD_SPEED = 50;     // Set the value between 0 and 255
const int BACKWARD_SPEED = 30;     // Set the value between 0 and 255
const int HORIZONTAL_SPEED = 50;  // Set the value between 0 and 255
const int DISTANCE_LOWER_BOUNDARY = 4;
const int DISTANCE_UPPER_BOUNDARY = 5.5;

void setup() {
  // This part is for setting up the Arduino pins and serial connection, please keep it unchanged
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  Serial.begin(9600);
  startMillis = millis();
}

// ******************************************************************************************************
void loop() {
distance_measurement();
}
// ******************************************************************************************************










// Function blocks that are used to control the robot
// ******************************************************************************************************
//void moving_forward() {
//  // Function that moves the robot forward
//  analogWrite(LEFT_MOTOR_PIN, FORWARD_SPEED);
//  analogWrite(RIGHT_MOTOR_PIN, FORWARD_SPEED);
//  digitalWrite(LEFT_MOTOR_PIN, LOW);
//  digitalWrite(RIGHT_MOTOR_PIN, LOW);
//}
//// ******************************************************************************************************
//void moving_backward() {
//  // Function that moves the robot backward
//  analogWrite(LEFT_MOTOR_PIN, BACKWARD_SPEED);
//  analogWrite(RIGHT_MOTOR_PIN, BACKWARD_SPEED);
//  digitalWrite(LEFT_MOTOR_PIN, HIGH);
//  digitalWrite(RIGHT_MOTOR_PIN, HIGH);
//}
//// ******************************************************************************************************
//}
//// ******************************************************************************************************
//void stop_moving_horizontal() {
//  analogWrite(HORIZONTAL_MOTOR_SPEED, 0);
//}
//void stop_moving_forward_backward() {
//  analogWrite(FORWARD_MOTOR_SPEED, 0);
//}
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
