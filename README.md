# Before you read
**This document is 80% complete, I will add more details when I got the time**

**Junzhe Chen**

**17th Aug. 2023**

**To-do list:**

**Add BoM**

**Fix the grammar**

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

## Movement control

### Little catch, Does the motor really work?

I have tested the motor with quite optimistic mindset for a bit, and found out it cannot drive the drone rather easily.

While the motor can perform well in empty load, it does not drive the wheel (which is around 20g of PLA print) that well. One reason is the small motor I used in this project is not designed to have great torque to drive heavy load. According to the datasheet shown below, it drives approximately 10g-cm torque, which is definitely a small value for driving the drone which have the total weight around 500g.

![motor's datasheet](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/6b51f472-45f1-42ae-9cbf-8c80952fe2e9)

While if I can use a continuous servo motor, the torque it can provide is much more greater, which can potentially provide better drivability for this project. A typical SG90 microservo can provide around 2.5kg-cm torque, which is **much** bigger (250 times) than the motor that is mentioned previously. The datasheet for the SG90 servo motor is shown below;
![microservo's datasheet](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/3953880c-ab2f-4ca1-bcb9-a18ecd7f14c2)

The microservo achieves that by applying gearbox inside the structure which by using small gear ratio, to increase the torque while reduce the speed. In this specific application, we don't need extremely fast spin of the motor (we are not making a fan or a propeller anyway), but we need generaous amount of torque to drive it forward. That is one reason why I decided to change the motor into microservo (which also saves plenty of breadboard space from the H bridge that is mentioned above).

### Working principle of the microservo

There are two types of microservo, one is those you can control the angle it turns between e.g. 0 and $180^\circ$, which is often used in the scenario where precision is the key. One good example of this type of microservo is [SG90](https://www.amazon.co.uk/s?k=sg90+servos&adgrpid=1185274432612636&hvadid=74079856314371&hvbmt=be&hvdev=c&hvlocphy=41471&hvnetw=o&hvqmt=e&hvtargid=kwd-74079786240109%3Aloc-188&hydadcr=14882_2226115&tag=mh0a9-21&ref=pd_sl_5ybxull069_e)

However, in this project, the key is to make the wheel moving constantly. In this case, we should consider using a continuous microservo. The working mechanism of those servo motor is when you give a short pulse duration (for example, 0.5ms), it will rotate in one direction; When you apply a longer pulse (for example, 1.5ms), it will rotate in the opposite direction. This satisfies the bare minimum requirements for making a movement control for a two wheel drone, as it should be sufficient with forward and backward motion with controllable speed. A typical continuous servomotor is [FS90R](https://www.rapidonline.com/Feetech-FS90R-360-Continuous-Rotation-Micro-Servo-37-1335?incVat=1&pdg=pla-4584070144863153:cmp-374690294:adg-1287528427140489:crv-:pos-:dev-c&msclkid=7fbfc0fe02f31d7be1caa41698b6734e&utm_source=bing&utm_medium=cpc&utm_campaign=Bing%20-%20UK%20-%20Shopping%20-%20Generic&utm_term=4584070144863153&utm_content=Shopping%20-%20Desktop%20-%20New%20feed)

in the Arduino library, we can include the servo header file, which provide the way to control most servo on the market. In my case, I am using the FS90R continuous microservo for my drone's movement. By using the header file, if I do `servo.write(90);` to my servo, it will stop and stay at neutral position. If I make the value smaller, it will constantly spin at one direction and same thing happen if I make the value bigger than 90, but the direction will be inverted. 

However, please take note that when we mount the motor to the breadboard, the two motor will be mirror to each other. Which means if both microservo have the same input (for example, `leftServo.write(80);` and `rightServo.write(80);` as the servo input), it will not go straight, but make the drone spin. In order to make the drone move forward, it should write the following:

```c++
void forward() {
  leftServo.write(90 - FORWARD_SPEED);
  rightServo.write(90 + FORWARD_SPEED);
}
```

But if I want the drone to spin around, I should write give the microservo the same input, like what is shown below:
```c++
void spin_left() {
  leftServo.write(90 - FORWARD_SPEED);
  rightServo.write(90 - FORWARD_SPEED);
}
```

## Choosing the battery

Interestingly, choosing the suitable battery for this project is the most time-consuming thing for me. The challenge in this part is to find a battery that can supply high current without too much voltage drop. The following picture shows that when the wheel is changing the direction, it will require high current up to around 0.8A to 1A. 

![Current](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/1feaaa3d-713c-4857-875a-7fe22f6d746b)

One interesting thing is when the battery is drawing too much current, the battery voltage will decrease a lot. The unfortunate truth is if the supply voltage run below 4V for the Arduino, the Arduino will turn off, therefore the program will not run. 

Initially, I used the 1604S 9V battery as it marked “super heavy duty”, but it turned out not as that “heavy duty”. When driving the battery with 1A of current, the voltage quickly dropped down below 1V. After reviewing the [datasheet](https://www.farnell.com/datasheets/612533.pdf) for that battery, it shows that the main focus for the battery is longevity, as it will last quite a long time compare with other 9V battery. The battery that I chose for my project is 1604A (the golden one shown below) which has better current drivability and it is designed to handle high current. 

![Batteries](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/659cc9fb-961e-40fd-9f41-a0d6f3378aba)


As it said in the introduction page at [Farnell](https://uk.farnell.com/gp-batteries/gp1604-10/battery-alkaline-pp3-pk10/dp/5057050):

> High power, long-life alkaline PP3 batteries giving up to six times the life of ordinary zinc carbon batteries; ideal for high drain applications
> 

which turned out to be good enough for this project.

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

According to my test, the ultrasonic sensor's readout is quite accurate. However, it does have some artifacts when the distance between the obstacle and the ultrasonic sensor is too close, which is why I put it around 2 or 3 centimeters inside of the drone to avoid the artifacts happens. The range of the ultrasonic sensor is from 2cm to 1 metre.
![ultrasonic measurement](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/4b448ac3-db85-401b-926f-2ba5b47ded67)

### Detailed look of obstacle avoidance logic with codes

To avoid the obstacle, we need to know few things:

- What is the threshold distance between the robot and the obstacle when you want the robot to avoid the obstacle?
- How does the robot determine whether it should turn left or turn right when facing an obstacle?

Which they sounds easy, but is actually a little tricky to implement when considering the limited space this design has.

As we all know for the ultrasonic sensor (which is the sensor chosen for this project), it only returns the distance in the front, but not the distance around the robot. In order to make the robot know the distance on the left and right side of the robot, there are the following few suggestions:

- Adding two more ultrasonic sensor, one on the left and another on the right. you will only have the gut of doing that if you have unlimited funds (Obviously, I am not), as well as plenty of space for the robot, which is not achievable on a small design like this.
- Mounting the ultrasonic sensor on a servo, making the sensor turn around to check the distance on the left and right hand side of the robot. It works quite like a sweeping radar, which can be quite interesting to make one one day. Unfortunately, there is not enough space for my project to implement this design.
- Rotating the robot to the left and the right to “peek” the distance. This solution is quite bad as it makes the robot constantly turning around. But due to the limited space for adding more components, this is the only way I can use.
- Surely there are more solutions other than that, maybe you can come up with better solution than me!

The flow chart for my version of obstacle avoidance logic is shown below:

![Flow chart](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/9fee4f43-ca53-4216-a3a8-4ed64b5650ae)

where if the sensor returns the distance value less than the threshold, the robot will first move backward to avoid collision, then checking the left and right distance between the robot and the obstacle, so that it can find the easiest path for it to pass through.

![Obstacle avoidance illustration](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/efe9a08f-7bdb-4ef2-8f69-e340400d9817)

As for the left and right peek, the flow chart and the code is shown below:

![flowchart int checkLeft](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/497fc3b9-21a8-42d0-874a-e67a73317781)

```cpp
int checkLeft() {
  STOP();
  delay(10);
  spin_left();
  delay(200);  // Calibrate this value till when doing one spin, the robot turns to 45 degrees
  distance_measurement();
  spin_right();
  delay(200); // Spin the robot back after checking left distance
  STOP();
  Serial.print(" >>> Distance to the left is: ");
  Serial.print(distance_cm);
  Serial.println("cm <<<");
  return distance_cm;
}
```

Same thing apply when checking the right side, the only difference will be that the `spin_left()` and `spin_right()` function will be inverted.

## Final demo

Here is a short video footage that shows how the obstacle avoidance function works

https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/eb08e50d-c34c-4a7a-859a-5a402ce13824

https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/b756a737-08ee-4902-a92b-56e6c9a67997

The **circuit diagram** of the breadboard is shown below, I use [TinkerCAD](www.tinkercad.com) to create it.

![Breadboard diagram](https://github.com/Junzhe-Chen/Two-wheels-breadboard-drone/assets/141964509/9c22b42d-8faa-41b5-9e26-340471e590be)

## Few notices and common mistakes

**Please check the following if your robot does not work, it might save you a day!**

1. The battery ground should share with the Arduino ground, if there are not sharing the same ground, the Arduino will be put in the floating state, therefore the voltage reference will not be accurate
2. Careful with your connection of the switch, the left and middle pin should connect to the positive Vin of the Arduino and the positive wire (red wire of the PP3) of the 9V battery. The negative input of the Arduino should not be connected to the switch. If the negative input is connected, when the switch is closed, it will **short** out the Arduino **which will make your unable to download, even worse, destroy your Arduino!**
3. When connecting the cable to the Arduino pin socket, make sure to make the sharp bend. This ensures good connection to the Arduino. You might encounter the robot behaves weirdly and ultrasonic sensor is constantly reading zero, this is usually caused by loose connection between Arduino and ultrasonic sensor
4. You may need to calibrate the microservo more or less. If the microservo does not stop given `servo.Write(90)`, spin the potentiometer at the back of the servo until its stops. Also, the speed of the left and right servo should be calibrated to the same, that makes the robot traveling straight when moving forward



