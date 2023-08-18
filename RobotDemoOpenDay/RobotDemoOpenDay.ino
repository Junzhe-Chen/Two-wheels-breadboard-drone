// **************************************************************************************************
// * This is the testing block for the robot                                                        *
// * It contains several functions that can make the robot move around and sensor to sense value    *
// * The function includes:                                                                         *
// * - moving_forward();                                                                            *
// * - moving_backward();                                                                           *
// * - moving_left();                                                                               *
// * - moving_right();                                                                              *
// * - button_state();                                                                              *
// * - distance_measurement();                                                                      *
// * Try to play around with the blocks by putting them in the loop, and see what will happen       *
// * For the sensor value, you can read them in the serial monitor.                                 *
// * You can open it by pressing "ctrl + shift + M" to see the sensor value                         *
// * Good luck and have fun playing around the robot!                                               *
// *                                                                    Junzhe Chen, Jun. 21st 2023 *
// **************************************************************************************************

// Constants (Do NOT change anything between the lines)
// **************************************************************************************************
const int LEFT_BUTTON_PIN = 4;
const int RIGHT_BUTTON_PIN = 5;
const int FORWARD_MOTOR_PIN = 12;
const int FORWARD_MOTOR_SPEED = 3;
const int HORIZONTAL_MOTOR_PIN = 13;
const int HORIZONTAL_MOTOR_SPEED = 11;
const int ULTRASONIC_ECHO_PIN = 6;
const int ULTRASONIC_TRIGGER_PIN = 7;
// Values that will be used in the program
int LEFT_BUTTON_STATE = 0;
int RIGHT_BUTTON_STATE = 0;
int DIRECTION_STATE = 0;  // 0 means going to the left, 1 means going to the right
float duration_us, distance_cm;
unsigned long startMillis;
unsigned long currentMillis;
// **************************************************************************************************
// Play around with the values below
const int OFFSET_TIME = 900;      // Offset time in millisecond
const int FORWARD_SPEED = 50;     // Set the value between 0 and 255
const int BACKWARD_SPEED = 30;     // Set the value between 0 and 255
const int HORIZONTAL_SPEED = 50;  // Set the value between 0 and 255
const int DISTANCE_LOWER_BOUNDARY = 4;
const int DISTANCE_UPPER_BOUNDARY = 5.5;

void setup() {
  // This part is for setting up the Arduino pins and serial connection, please keep it unchanged
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(FORWARD_MOTOR_PIN, OUTPUT);
  pinMode(HORIZONTAL_MOTOR_PIN, OUTPUT);
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  Serial.begin(9600);
  startMillis = millis();
}

// ******************************************************************************************************
void loop() {
button_state();
distance_measurement();
horizontal_movement();
forward_backward_movement();

}
// ******************************************************************************************************










// Function blocks that are used to control the robot
// ******************************************************************************************************
void moving_forward() {
  // Function that moves the robot forward
  analogWrite(FORWARD_MOTOR_SPEED, FORWARD_SPEED);
  digitalWrite(FORWARD_MOTOR_PIN, LOW);
}
// ******************************************************************************************************
void moving_backward() {
  // Function that moves the robot backward
  analogWrite(FORWARD_MOTOR_SPEED, BACKWARD_SPEED);
  digitalWrite(FORWARD_MOTOR_PIN, HIGH);
}
// ******************************************************************************************************
void moving_left() {
  // Function that moves the robot to the left
  analogWrite(HORIZONTAL_MOTOR_SPEED, HORIZONTAL_SPEED);
  digitalWrite(HORIZONTAL_MOTOR_PIN, LOW);
}
// ******************************************************************************************************
void moving_right() {
  // Function that moves the robot to the right
  analogWrite(HORIZONTAL_MOTOR_SPEED, HORIZONTAL_SPEED);
  digitalWrite(HORIZONTAL_MOTOR_PIN, HIGH);
}
// ******************************************************************************************************
void stop_moving_horizontal() {
  analogWrite(HORIZONTAL_MOTOR_SPEED, 0);
}
void stop_moving_forward_backward() {
  analogWrite(FORWARD_MOTOR_SPEED, 0);
}
// ******************************************************************************************************
void button_state() {
  LEFT_BUTTON_STATE = digitalRead(LEFT_BUTTON_PIN);
  RIGHT_BUTTON_STATE = digitalRead(RIGHT_BUTTON_PIN);
  if (LEFT_BUTTON_STATE == HIGH) {
    Serial.println("Left button is pressed");
    DIRECTION_STATE = 1;
  }
  if (RIGHT_BUTTON_STATE == HIGH) {
    Serial.println("right button is pressed");
    DIRECTION_STATE = 0;
  }
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





// The code that can perform the maze solving 
// ******************************************************************************************************
void horizontal_movement() {
  if (distance_cm < DISTANCE_UPPER_BOUNDARY) {  // no horizontal movement when move thru the gap
    // Make the robot move to the right when the left side hit the wall
    if (DIRECTION_STATE == 0) {
      // Serial.println("Moving to the right >>>");
      moving_right();
    }
    // Make the robot move to the left when the right side hit the wall
    else if (DIRECTION_STATE == 1) {
      // Serial.println("Moving to the left >>>");
      moving_left();
    }
  } 
}

// ******************************************************************************************************
void forward_backward_movement() {
  Serial.println(distance_cm);
  if (distance_cm > DISTANCE_UPPER_BOUNDARY) {
    if (currentMillis - startMillis >= OFFSET_TIME) {
      moving_forward();  //  Moving forward through the gap
      stop_moving_horizontal();
    }
  } else if (distance_cm < DISTANCE_LOWER_BOUNDARY) {
    startMillis = currentMillis;  // Update the timer
    moving_backward();
    // Robot adjusts its position backward
  } else {
    startMillis = currentMillis;
    stop_moving_forward_backward();
  }
}


