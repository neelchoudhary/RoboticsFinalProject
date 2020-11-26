#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;

#include "robot.hh"
#include "ranger-library.hh"

RgRobot::RgRobot(int argc, char* argv[], void (*cb)(Robot*))
    : on_update(cb)
{
    create_robot_connection("/dev/ttyUSB0");
}

RgRobot::~RgRobot()
{

}

std::array<double, 5> 
RgRobot::read_all_sensors() {
  return read_all();
}

int
RgRobot::get_line_status(){
  return read_line_sensor();
}

int
RgRobot::get_line_status2(){
  return read_line_sensor();
}

double
RgRobot::get_noise_sensor(){
  return read_sound_sensor();
}

void
RgRobot::set_vel(double lvel, double rvel)
{
    tank_drive(rvel, lvel);
}

void
RgRobot::do_stuff()
{
    while (1) {
        // cout << "\n == iterate ==" << endl;
        this->on_update(this);
    }
}
