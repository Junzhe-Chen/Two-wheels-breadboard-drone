# Before you read
**This document is still in the early development stage, we will keep updating and hopefully finish the first iteration of this project before next week**

**Junzhe Chen**

**17th Aug. 2023**


# Overview

The two wheel car is built onto a small breadboard, with an Arduino Uno attached at the bottom. There are two small DC motor which acts as the driver of the motor each with a dedicate full H bridge to control it. The robot also contains an ultrasonic sensor that is used to avoid obstacles. 

# Basic structure

The motor is composed of 4 parts:

- 9V battery as the power source

![PXL_20230817_164713843.jpg](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/0b25c2a4-79f2-4fc7-ab1b-9d2189e162ba/PXL_20230817_164713843.jpg)

- Arduino Uno R3 for the control

![PXL_20230817_164914599.jpg](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/2825525e-5cb5-41b4-a0e1-02e40fcc6559/PXL_20230817_164914599.jpg)

- Breadboard for attaching electrical components

![PXL_20230817_164824961.jpg](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/added57a-2062-4edc-86ad-b1ac15f71bda/PXL_20230817_164824961.jpg)

- motors that makes the robot move

![PXL_20230817_164800716.jpg](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/cec0afee-a7b5-4e06-b92b-3dc679c992b1/PXL_20230817_164800716.jpg)

I tried to print out the wheel on a 3D printer, then I screwed up…

![PXL_20230817_163803676.jpg](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/dcc45d73-99c1-419b-a6cd-ad0122175a4d/PXL_20230817_163803676.jpg)

If the wheel is designed to be 3D printed out, one thing to keep notice that is the wheel should have abundance contact with the plate of the 3D printer, which ensures the print stays in its place while printing. My first print has relatively thin structure (to save material) which does not provide enough contact area to the plate, causing the print to fail eventually. 

In my second design, I make the wheel more beefy and so far the print is going OK.

One of the objective of the project is to make the robot 

- able to move forward, backward, left and right
- can avoid obstacle (if there is a obstacle in the front of the robot, it should be able to go around and pass it)
- having the lighting in the front which can be turned on and off by the Arduino

## Motor control

DC motor is relatively easy to control, if you provide the rated voltage across it, it will spin. If you switch the direction, it will spin in the opposite direction. However, there are few things that need to be taking care off

- DC motor is an inductive load, the inductive voltage kick might destroy the control circuit
    - Flyback diode connecting in parallel with the motor
- DC motor can only be powered properly under rated voltage (in this case, 5V). If the motor is powered below 5V, it might not be able to spin properly
    - Controlling the motor speed by PWM, I have written an [article](https://github.com/Junzhe-Chen/PWM-Modulator-Circuit) about that, check for further detail
- A circuit is required to determine which direction the motor would need to spin
    - H bridge circuit (in the next session) [insert picture]

### Working principle of H bridge

H bridge is a useful circuit that can change the polarity of the component connecting in the middle of the bridge. [insert many figures and explanations]

### Inductor “kick” and flyback diode

Inductor is a quite interesting component, which when a current passing through it, magnetic field around the inductor began to build up. If there is a sudden change of current such as suddenly cutting off the voltage across the inductor, the magnetic field will collapse and turn it into current. The fundamental principle for inductor can be quite hard to explain, for the math fan here, the current voltage relationship on a inductor can be written as  

$$
v_L=L\frac{di_L}{dt}
$$

It can be understood as inductor doesn’t like sudden change, in another word, resists change. If you apply sudden change to an inductor, it will kick you back really REALLY hard. There is actually a law that define this characteristics called [Lenz’s Law](https://en.wikipedia.org/wiki/Lenz%27s_law).

However, sometimes we DO need to make instance change in voltage across the motor (which is mainly inductive), then where should those kickback voltage go? That is when the use of Flyback diode comes in [insert picture in ******************Art of Electronics 3rd Edition p. 39******************] 

![Untitled](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/85c7c55f-308a-442e-b645-1d273a291553/Untitled.png)

Figure 1.84 shows a good example of the flyback diode. This basically gives the vague idea of where the diode should be in the H bridge. Do consider about the motor works in both polarity so in total of 4 diodes is needed to fully protect the transistors not being blown up by the motor’s inductive kick.

## Obstacle avoiding logic

In order to make the robot avoid obstacle, we need some sort of sensor that can be used to detect the distance between the robot and the surrounding. For this specific use case, the following sensor can be chosen:

- Ultrasonic sensor
    - Relatively cheap and easy-to-use, limited range (till around 2m), limited speed, take extra caution about the glitch when the sensor is too close to the wall
- IR sensor
    - Great accuracy and speed, limited range (depending to the ambient light) and light dependant, not that great for this use case
- LiDAR
    - [GreatScott!! video](https://youtu.be/_--8K1BW_O4)
    - adaptive scanning (sounds cool)

In this documentation, I will focus on the ultrasonic sensor one, because the sensor is readily available in my university lab and I had past experience with that. In the future, I might consider using LiDAR and according to GreatScott, it is definitely doable and it can give much more accurate control (just look at the commercial sweeping robot, they use LiDAR to avoid obstacle, meanwhile, map the room)

### Basics of ultrasonic sensor

As we all know, sound travels around $340m/s$ in the air. Remember this data, which will be quite useful in the future.

[insert image for better explanation]

 The principle of the ultrasonic sensor is to send a short pulse in certain time period, and receive the same pulse with the receiver. By calculating the time difference between sending and receiving signal, the distance can be calculated by the following equations:

$$
d=\frac{340}{2t}
$$

where $d$ represents the distance between the sensor and the obstacle, $t$ is the time between the pulse being sent and the time where signal being received.

By using this principle, we can write following codes in Arduino to sense the distance between the sensor and the obstacle.

```arduino
void distance_measurement() {
    if (millis() > currentMillis + 100) {  // Distance measurement in every 100 milliseconds
      currentMillis = millis();
     
			// generate 10-microsecond pulse to TRIG pin
      digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
      delayMicroseconds(10); 
      digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);

      // measure duration of pulse from ECHO pin
      duration_us = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);

      // calculate the distance
      distance_cm = 0.017 * duration_us; // Formular that is mentioned before
      Serial.println("distance: ");
      Serial.print(distance_cm);
      // return distance_cm;
      // print the value to Serial Monitor
    }
  }
```
