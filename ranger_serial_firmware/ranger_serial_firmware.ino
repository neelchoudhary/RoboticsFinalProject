#include "MeAuriga.h"
#include <SoftwareSerial.h>
#include <Wire.h>

using namespace std;


MeLightSensor light_sensor(2);
MeSoundSensor sound_sensor(5);
MeGyro gyro_sensor(1, 0x69);
MeTemperature temperature_sensor(PORT_13);
MeUltrasonicSensor sonar_sensor(9);
MeLineFollower line_sensor(10);
MeLineFollower line_sensor2(8);
MeEncoderOnBoard right_motor(SLOT1);
MeEncoderOnBoard left_motor(SLOT2);
MeBuzzer buzzer;
MeRGBLed led;

int light = 0;

char commandArray[64];
char cmd_idx = 0;

void poll_serial()
{
    while (Serial.available()) {
        char cc = Serial.read();
        if (cc == '\n') {
            commandArray[cmd_idx] = 0;
            return;
        }
        else {
            commandArray[cmd_idx++] = cc;
        }
    }
}

void setup()
{
  Serial.begin(9600);
  gyro_sensor.begin();
  buzzer.setpin(45);
  led.setpin(44);
}

void loop()
{

  Serial.println(String(line_sensor.readSensors()) + ":" + String(line_sensor2.readSensors()) + ":" + String(gyro_sensor.getAngleX()) + ":" + String(gyro_sensor.getAngleY()) + ":" + String(gyro_sensor.getAngleZ()) + ":");

  poll_serial();
  
  gyro_sensor.update();

  String command = commandArray;

  int argumentCount = 0;
  for(int i = 0; i < command.length(); i++){
    if(command.charAt(i) == ' '){
      argumentCount++;
      
    }
  }
  
  String commandVerb = "";
  String arguments[argumentCount];

  if(argumentCount > 0){
    command += " ";
    commandVerb = command.substring(0, command.indexOf(" "));
    command = command.substring(command.indexOf(" ") + 1);
    int count = 0;
    while(count < argumentCount){
      arguments[count] = command.substring(0, command.indexOf(" "));
       count++;
       command = command.substring(command.indexOf(" ") + 1);
    } 
  }

  switch(argumentCount){
    case 0:
      if (command.equals("read_light")){
         Serial.println(light_sensor.read());
      } else if (command.equals("read_sound")){
          Serial.println(sound_sensor.strength());
      } else if (command.equals("read_gyro_x")){
          Serial.println(gyro_sensor.getAngleX());
      } else if(command.equals("read_gyro_y")){
          Serial.println(gyro_sensor.getAngleY());
      } else if(command.equals("read_gyro_z")){
          Serial.println(gyro_sensor.getAngleZ());
      } else if(command.equals("read_temperature")){
          Serial.println(temperature_sensor.temperature());
      } else if(command.equals("read_sonar")){
          Serial.println(sonar_sensor.distanceCm());
      } else if(command.equals("read_line")){
          // buzzer.tone(1000, 100);
          Serial.println(line_sensor.readSensors());
      } else if(command.equals("stop")){
          right_motor.setMotorPwm(0);
          left_motor.setMotorPwm(0);
      } else if(command.equals("forward")){
          right_motor.setMotorPwm(-200);
          left_motor.setMotorPwm(200);
      } else if(command.equals("left")){
          right_motor.setMotorPwm(-200);
          left_motor.setMotorPwm(-200);
      } else if(command.equals("right")){
          right_motor.setMotorPwm(200);
          left_motor.setMotorPwm(200);
      } else if(command.equals("backward")){
          right_motor.setMotorPwm(200);
          left_motor.setMotorPwm(-200);
      } else if(command.equals("light_off")){
          led.setColor(0, 0, 0, 0);
          led.show();
      }
      
      break;
      
    case 1:
      if(commandVerb.equals("forward")){ 
         int speed = arguments[0].toInt();
         right_motor.setMotorPwm(-speed);
         left_motor.setMotorPwm(speed);
      } else if(commandVerb.equals("left")){
         int speed = arguments[0].toInt();
         right_motor.setMotorPwm(-speed);
         left_motor.setMotorPwm(-speed);
      } else if(commandVerb.equals("right")){
         int speed = arguments[0].toInt();
         right_motor.setMotorPwm(speed);
         left_motor.setMotorPwm(speed);
      } else if(commandVerb.equals("backward")){
         int speed = arguments[0].toInt();
         right_motor.setMotorPwm(speed);
         left_motor.setMotorPwm(-speed);
      } else if(commandVerb.equals("light_off")){
         int index = arguments[0].toInt();
         led.setColor(index, 0, 0, 0);
         led.show();
      }
      
      break;

    case 2:
      if (commandVerb.equals("sound")) {
        int frequency = arguments[0].toInt();
        int duration = arguments[1].toInt();
        buzzer.tone(frequency, duration);
      } else if (commandVerb.equals("tank_drive")){
        int right = arguments[0].toInt();
        int left = arguments[1].toInt();
        right_motor.setMotorPwm(-right);
        left_motor.setMotorPwm(left);
      }

      break;

    case 3:
      if (commandVerb.equals("light")){
        int red = arguments[0].toInt();
        int green = arguments[1].toInt();
        int blue = arguments[2].toInt();
        led.setColor(0, red, green, blue);
        led.show();
      }

      break;

    case 4:
      if (commandVerb.equals("light")){
        int index = arguments[0].toInt();
        int red = arguments[1].toInt();
        int green = arguments[2].toInt();
        int blue = arguments[3].toInt();
        led.setColor(index, red, green, blue);
        led.show();
      }

      break;
   }

   commandArray[0] = 0;
   memset(commandArray, 0, sizeof(commandArray));
   cmd_idx = 0;
   
   delay(50);
   
}
