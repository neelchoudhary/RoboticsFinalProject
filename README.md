# Maze Builder Tutorial/Setup - Extra Credit Attempt

Building a maze in Gazebo is tedious. Our group has documented an easier way to build a maze in Gazebo.

Unlike the existing Gazebo maze generator, this builder was specific for our group's need for a maze suitable for line following. Using both the physical and digital robots as references, we constructed this maze to work for the ultrasonic sensor(s).

In order to begin building, you will first need to construct suitable line segments for your robot. We have provided two types of line segments for horizontal and vertical lines in `/models`.

You can view our sample maze created using the maze builder in `/worlds`.


## Steps
1. Open up an empty/pre-existing world in the Gazebo environment with `world.sh`.
2. Navigate to the Insert tab at the top-left corner.
3. Select a model underneath `$PATH/models` and drag to the Gazebo environment.
4. Save the world by going to `File >> Save World As` and overwriting the world environment.