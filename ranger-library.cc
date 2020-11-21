#include <stdio.h>
#include <iostream>
#include <SerialStream.h>
#include <unistd.h>

#include "ranger-library.hh"

using namespace LibSerial;
using namespace std;

static SerialStream port;

string command;

void create_robot_connection(std::string portAddress){
  port.Open(portAddress);
  port.SetBaudRate(LibSerial::BaudRate::BAUD_9600);
  port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  port.SetParity(LibSerial::Parity::PARITY_NONE);
  port.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
}

void set_all_lights(int r, int g, int b){
  command = "light " + to_string(r) + " " + to_string(g) + " " + to_string(b);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void set_single_light(int index, int r, int g, int b){
  command = "light " + to_string(index) + " " + to_string(r) + " " + to_string(g) + " " + to_string(b);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void lights_off(){
  command = "light_off";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void single_light_off(int index){
  command = "light_off " + to_string(index);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void move_forward(){
  command = "forward";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void move_backward(){
  command = "backward";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void turn_left(){
  command = "left";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void turn_right(){
  command = "right";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void move_forward(int speed){
  command = "forward " + to_string(speed);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void move_backward(int speed){
  command = "backward " + to_string(speed);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void turn_left(int speed){
  command = "left " + to_string(speed);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void turn_right(int speed){
  command = "right " + to_string(speed);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void stop(){
  command = "stop";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void tank_drive(int rspeed, int lspeed){
  command = "tank_drive " + to_string(rspeed) + " " + to_string(lspeed);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

void make_noise(int frequency, int duration){
  command = "sound " + to_string(frequency) + " " + to_string(duration);
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
}

double read_line_sensor(){
  command = "read_line";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
  return stod(read_serial());
}

double read_light_sensor(){
  command = "read_light";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
  return stod(read_serial());
}

double read_temperature_sensor(){
  command = "read_temperature";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
  return stod(read_serial());
}

double read_sound_sensor(){
  command = "read_sound";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
  return stod(read_serial());
}

double read_sonar_sensor(){
  command = "read_sonar";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
  return stod(read_serial());
}

double read_gyroscope_x(){
  command = "read_gyro_x";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
  return stod(read_serial());
}

double read_gyroscope_y(){
  command = "read_gyro_y";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
  return stod(read_serial());
}

double read_gyroscope_z(){
  command = "read_gyro_z";
  port.write(command.c_str(), command.length());
  port.DrainWriteBuffer();
  return stod(read_serial());
}

std::array<double, 5> read_all() {
  string sensorReading = read_serial();
  size_t pos = 0;
	double token;
  
  std::array<double, 5> sensorReadings;
	// double sensorReadings[5];
	int sensorCount = 0;
	while ((pos = sensorReading.find(":")) != string::npos) {
			token = stod(sensorReading.substr(0, pos));
			sensorReadings[sensorCount] = token;
			sensorCount = sensorCount + 1;
			sensorReading.erase(0, pos + 1);
	}
  return sensorReadings;
  // return stod(read_serial());
}

std::string read_serial() {
  char temp[100];
  int ii = 0;
  for(; ii < 96; ++ii){
    port.read(temp + ii, 1);
    if(temp[ii] == '\n') {
      break;
    }
  }

  temp[ii+1] = 0;
  string data = temp;
  return data;
}