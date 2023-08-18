# Before you read
**This document is still in the early development stage, we will keep updating and hopefully finish the first iteration of this project before next week**

**Junzhe Chen**

**17th Aug. 2023**


# Overview

The two wheel car is built onto a small breadboard, with an Arduino Uno attached at the bottom. There are two small DC motor which acts as the driver of the motor each with a dedicate full H bridge to control it. The robot also contains an ultrasonic sensor that is used to avoid obstacles. 

# Basic structure

The drone looks like this:

![Overview of the drone](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/d2e335e0-daa4-4085-9b6e-9183a21fcb0b)

The drone is composed of 4 parts:

- 9V battery as the power source

![9V battery power](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/c2495dcd-1f37-479c-8c85-64f0554d14f3)

- Arduino Uno R3 for the control

![Arduino UNO](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/cde8a05c-6a6d-4efb-8732-69b346429bc8)

- Breadboard for attaching electrical components

![Breadboard](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/cc81d78b-4a6b-4593-badd-5b5cbb51727e)

- motors that makes the robot move

![Motor](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/ec2e96c8-bb13-4be1-ae9d-b7642e728f4e)

I tried to print out the wheel on a 3D printer, then I screwed up… **Bruh**

![Failed 3D printing residue](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/ba81d655-7e75-453b-8d18-e79dcfe0618b)

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

H bridge driver
![H bridge overview](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/52ffa30f-411e-4416-baa6-a3185be22bc6)

H bridge, M1 and M4 conducting
![H bridge case 1](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/e42908ea-c051-4bfc-aaef-6c575560ac44)

H bridge, M2 and M3 conducting
![H bridge case 2](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/8841472c-6fc0-4e51-8571-71b0ba703bf7)

H bridge, STOP
![H bridge case STOP](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/67e44c97-c2d9-426a-9515-2d747f8d9e9d)

H bridge with potection diode
![H bridge flyback diode](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/a7cf1098-ed70-4a7f-9c05-377c38c7f392)

| A    | B    | Direction     |
|------|------|---------------|
| LOW  | LOW  | BRAKE         |
| LOW  | HIGH | CLOCKWISE     |
| HIGH | LOW  | ANTICLOCKWISE |
| HIGH | HIGH | BRAKE         |


### Inductor “kick” and flyback diode

Inductor is a quite interesting component, which when a current passing through it, magnetic field around the inductor began to build up. If there is a sudden change of current such as suddenly cutting off the voltage across the inductor, the magnetic field will collapse and turn it into current. The fundamental principle for inductor can be quite hard to explain, for the math fan here, the current voltage relationship on a inductor can be written as  

$$
v_L=L\frac{di_L}{dt}
$$

It can be understood as inductor doesn’t like sudden change, in another word, resists change. If you apply sudden change to an inductor, it will kick you back really REALLY hard. There is actually a law that define this characteristics called [Lenz’s Law](https://en.wikipedia.org/wiki/Lenz%27s_law).

However, sometimes we DO need to make instance change in voltage across the motor (which is mainly inductive), then where should those kickback voltage go? That is when the use of Flyback diode comes in. The following picture from *Art of Electronics* gives a intuitive explanation of why we need a flyback diode for inductive load:

![flyback diode art of electronics](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/25933043-e56f-4ef5-8909-7fe1270b7b2f)

> When the switch is opened, the inductor tries to keep current flowing from A to B, as it had been. In other words, it tries to make current flow out of B, which it does by forcing B to a high positive voltage (relative to A). In a case like this, in which there’s no connection to terminal B, it may go 1000 V positive before the switch contact “blows over.” This shortens the life of the switch and also generates impulsive interference that may affect other circuits nearby. If the switch happens to be a transistor, it would be an understatement to say that its life is shortened; its life is ended. Horowitz, Paul *Art of Electronics, p.39*
>

Figure 1.84 shows a good example of the flyback diode. This basically gives the vague idea of where the diode should be in the H bridge. Do consider about the motor works in both polarity so in total of 4 diodes is needed to fully protect the transistors not being blown up by the motor’s inductive kick.

## Obstacle avoiding logic

In order to make the robot avoid obstacle, we need some sort of sensor that can be used to detect the distance between the robot and the surrounding. For this specific use case, the following sensor can be chosen:

- Ultrasonic sensor
    - Relatively cheap and easy-to-use, limited range (till around 0.5m), limited speed, take extra caution about the glitch when the sensor is too close to the wall
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

```c++
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
