#ifndef RGLIBRARY_HH
#define RGLIBRARY_HH

#include <string>
#include <array>

/*
This method needs to be first in order to establish the serial connection
to the robot.
*/
void create_robot_connection(std::string portAddress);

/*
  Sets all the lights one color in rgb format.
*/
void set_all_lights(int r, int g, int b);

/*
  Sets a single light one color in rgb format. Index is the light that will be
  set, the avialable lights are numbered 1-12.
*/
void set_single_light(int index, int r, int g, int b);

/*
  Turns all of the lights off.
*/
void lights_off();

/*
  Turns a single light off. Index is the light that will be
  turned off, the avialable lights are numbered 1-12.
*/
void single_light_off(int index);

/*
  Moves the robot forward at the default speed.
*/
void move_forward();

/*
  Moves the robot backward at the deault speed.
*/
void move_backward();

/*
  Rotate the robot to its left at the default speed.
*/
void turn_left();

/*
  Rotate the robot to its right at the default speed.
*/
void turn_right();

/*
  Move the robot forward at the speed taken as input. Recommended values are
  100 < speed < 255 and -255 < speed < -100.
*/
void move_forward(int speed);

/*
  Move the robot backward at the speed taken as input. Recommended values are
  100 < speed < 255 and -255 < speed < -100.
*/
void move_backward(int speed);

/*
  Turn the robot to its left at the speed taken as input. Recommended values are
  100 < speed < 255 and -255 < speed < -100.
*/
void turn_left(int speed);

/*
  Turn the robot to its right at the speed taken as input. Recommended values are
  100 < speed < 255 and -255 < speed < -100.
*/
void turn_right(int speed);

/*
  Stop the the robots movement.
*/
void stop();

/*
  Use tank drive style movement on the robot. rspeed is the speed of the right tread
  and lspeed is the speed of the left tread. Recommended values are
  100 < speed < 255 and -255 < speed < -100.
*/
void tank_drive(int rspeed, int lspeed);

/*
  Trigger the robot's buzzer. The frequency and duration of the sound are taken
  as the parameters. The duration is in milliseconds.
*/
void make_noise(int frequency, int duration);

/*
  Read the value from the line sensor. Values are 1, 2, 3.
  1 means sensor 1 is over the line
  2 means sensor 2 is over the line
  3 means both sensors are over the line
*/
double read_line_sensor();

/*
  Read the light sensor value.
*/
double read_light_sensor();

/*
  Read the temperature sensor value in Clesius.
*/
double read_temperature_sensor();

/*
  Read the sound sensor value. Returns the strenth of the sound.
*/
double read_sound_sensor();

/*
  Read the sonar sensor value. Max dist is 400cm
*/
double read_sonar_sensor();

/*
  Read the gyroscope x value.
*/
double read_gyroscope_x();

/*
  Read the gyroscope y value.
*/
double read_gyroscope_y();

/*
  Read the gyroscope z value.
*/
double read_gyroscope_z();

std::array<double, 5> read_all();

/*
  Read the data in the serial if it exists.
*/
std::string read_serial();




#endif
