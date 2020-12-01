# Maze Solver Control Program
Neel Choudhary and Nicole Danuwidjaja

## Project Proposal
Goal: Write a robot control program to successfully solve a maze at a given point using line following and sonar sensing.
- Robot moves autonomously using line following techniques to navigate through maze
- Identify surroundings using sonar sensor to find next best path
- Use sonar sensor to identify the goal.

## Components
The mBot Ranger comes with an ultrasonic sonar sensor, a line follower sensor, two motors for movement, and the MeAuriga Arduino motherboard. 
Our robot is modded with an additional line sensor.



### Breakdown
1. Construct physical mBot Ranger Robot
2. Establish computer/mBot serial communication
3. Connect firmware and write functional code
4. Determine maze layout and build physical course
5. Create models to replicate maze in Gazebo

### Structure
`/brain` contains mBot Ranger libraries, Robot classes, and control program for virtual/physical bots

`/models` Gazebo model files with plugins to represent objects virtually

`/plugins` Shared Gazebo libraries for models and elements

`/ranger_serial_firmware` installation of MeAuriga firmware Sketch

`/ranger_test` control program + firmware Sketch


### Usage
1. `./world.sh`
2. `./run` in separate terminal