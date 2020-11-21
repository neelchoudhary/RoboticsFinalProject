# 🤖 mBot Ranger firmware and c++ library for USB serial communication.
## Description
This library provides functions that allow for access to all actuators and sensors
on the mBot Ranger robot. It opens a serial port connection given a port address.
Then has functions to control the lights, buzzer, and movement of the robot. It has
a read function for each of the sensors on the robot that return the value communicated
from the robot. This directory also has a firmware file that works with the library.

## Instructions:
Plug the sonar sensor into port 9 and the line sensor into port 10 on the mBot Ranger.

Load the ranger_serial_firmware.ino file on to your mBot Ranger arduino board.

Download [LibSerial](https://libserial.readthedocs.io/en/latest/index.html#). Or use
    ```
    sudo apt install libserial-dev
     ```
     
     
     * NOTE: You may have to manually set the path for libserial in CMakeLists.txt as seen in this CMakeLists.txt file.

Copy ranger-library.cc and ranger-library.hh into your project. Then include the header
file in the file of your choosing. Make sure to call create connection before any other functions. Then you can use any of the methods to communicate with the robot.

For convenience, there is an example program provided. You can run make on this directory and then run the brain executable. This will make the robot run all of
its commands and print a reading from each of the sensors. **Warning, the robot will move around so make sure it is an open space and the cord is free to move with the robot.**

Contributions and suggestions for improvement are welcome.


Attributions:

https://github.com/NatTuck/cs5335hw-gazebo/tree/master/ranger. by Professor Nat Tuck Northeastern November 2020

 https://github.com/nbock/cs5335-nm/tree/master by Nolan Bock Northeastern November 2020

 https://docs.google.com/document/d/16uXDUmgN_9jM2sp_KGJtZZfQTpQ2-PzLDtjUFla_FcA/edit by Victor_Leung
 
 https://docs.google.com/document/d/1EpMWJo9pP2J_pstzXA-XHK8t00Z70SCZYwZ_Kl7VLuw/edit#heading=h.uop2tonh9y21 by Murray Elliot
 
 https://github.com/Makeblock-official/Makeblock-Libraries Examples by Makeblock
