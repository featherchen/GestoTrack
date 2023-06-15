# GestoTrack [[Link]](https://github.com/featherchen/GestoTrack)

## Introduction
GestoTrack is a gesture tracking system built on STM32 B-L475E-IOT01A2 with Mbed OS and HC-SR04 ultrasonic sensors.

This is also a Final Project of Electrical Engineering Lab (embedded System), NTU, 2023Spring.

## Motivation
Just like [MightyRC](https://mightyrc.com/), we want to create a system that can detect gestures and control related IoT devices.
## Architechture

This Project is mainly composed of [STM32 B-L475E-IOT01A2](https://www.st.com/en/evaluation-tools/b-l475e-iot01a.html) (L475) and [Raspberry Pi](https://www.raspberrypi.com/) (Rpi). In order to get the gesture data, we connect the former with HC-SR04 distance sensor, see sensor section below. The latter is a powerful board that can help us to control those IoT devices. In the experiment, we take LED and buzzer as an example to show the concept of GestoTrack, i.e. proof of concept. Our solution can be easily applied to more complicated devices as long as they are compatible with Rpi interface.

We choose WiFi as the bridge between L475 and Rpi. The first reason is that MightyRC uses WiFi. Second, WiFi has a longer connection distance and fewer security issues. Last but not least, the complexity of implementing Wifi communication between L475 and Rpi is much lower than Bluetooth, reducing the possibility of unexpected issues or bugs.
## Sensor

The sensor works as follows:

1. To initiate a detection, the trigger pin should be held at voltage high for at least 10 microseconds.
2. After Step 1., the sensors emit 8 pulses of ultrasonic wave at a frequency of 40kHz.
3. At the end of step 2., the echo pin is changed to voltage high.
4. When the sensors receive an ultrasonic wave, the echo pin is changed to low. Thus the distance to the first object, which caused the wave to bounce back, is proportional to the duration in which the echo pin is at voltage high.

Due to the nature of time-of-flight based detection, multiple sensors are difficult to synchronize. 

In particular, we used ISR and the TimeOut class to repeatedly execute the detecting function per $timeout$ seconds. This parameter needs to be fine-tuned. It can not be too long because:
1. It might fail to detect the object if the object moves in the middle of *timeout*.

It can not be too short because:
1. It can not be shorter than the time needed to perform a full detection cycle. This time is dependent of the distance to the detected object because of the nature of time-of-flight detection.

## Detection Algorithm

### Definition of Gesture

Assume that there is only one object moving in front of the sensors and the sensor does not interfere. A gesture is defined as **an order of sensors that detected the object**.

Since the whole program involves ISR and the sensors are not perfectly synchronized, we use a buffer to store past data for every sensor. *main()* then uses the data to solve for gestures.

### Data structure

We used a queue-like data structure to store limited past data for each sensor. It acts as a queue, but the size will not exceed a predetermined constant. It is implemented with a simple array, thus the queue is similar to a sliding window.

Following guidelines determine the size of the array and the window width:

1. The larger the array size, the more memory each sensor will use, but the less number of times data need to be moved from the back to the front.
2. The width essentially determines the complexity of gestures to detect.

### Gesture Recognition
![image](https://github.com/featherchen/GestoTrack/blob/main/resource/state%20diagram.png?raw=true)


State0: Idle state, ready for gesture detection.
State3: Final state, back to state0 for next detection.

We have two sensors, sensor1 and sensor2. the number on each arc means the corresponding sensor detected an object in front of it. For instance, when sensor1 detects an object at state0, the state changes to state1.

At each state, if it does not change to the next state for a consecutive time(i.e. *timeout*), the state should change to state0, which also tells us this gesture is finished.

Take state21 as an example, if it goes to state0 due to timeout, the gesture should should be `2->1`.

The value of *timeout* is a parameter that should be set properly, in our prototype *timeout* is 1 second for each state except state3. Since state3 is the final state, *timeout* can be smaller, e.g. 0.2 seconds.

## Connect to RPi via WiFi
Create a socket server at Rpi, whenever L475 recognizes a gesture, it sends an encoded message to the server. Rpi controls the device (e.g. make the LED blink, let the buzzer sound) based on the received message.
## Demo Video
https://youtu.be/BlcjqGm-lUg 

## Slides
* [Proposal](https://github.com/featherchen/GestoTrack/blob/main/resource/Proposal.pdf)
* [Progress Report](https://github.com/featherchen/GestoTrack/blob/main/resource/Progress%20Report.pdf)
* [Final Presentation](https://github.com/featherchen/GestoTrack/blob/main/resource/Final%20Presentation.pdf)
## Reference

[How HC-SR04 Ultrasonic Sensor Works & Interface It With Arduino](https://lastminuteengineers.com/arduino-sr04-ultrasonic-sensor-tutorial/)
[HC_SR04_Ultrasonic_Library](https://os.mbed.com/users/ejteb/code/HC_SR04_Ultrasonic_Library/#e0f9c9fb4cf3d5b7213e38d0aa18bda0abcfaa32)
[HC-SR04 manual](https://web.eece.maine.edu/~zhu/book/lab/HC-SR04%20User%20Manual.pdf)
[Bluetooth v.s. WiFi](https://www.mokoblue.com/zh-tw/bluetooth-vs-wifi-which-is-better/)
