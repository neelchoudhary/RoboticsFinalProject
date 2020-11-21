#include "MeAuriga.h"
#include <SoftwareSerial.h>
#include <Wire.h>

MeLightSensor light_sensor(2);
MeSoundSensor sound_sensor(5);
MeGyro gyro_sensor(1, 0x69);
MeTemperature temperature_sensor(PORT_13);
MeUltrasonicSensor sonar_sensor(9);
MeLineFollower line_sensor(10);
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

int lights[1000];


void setup()
{
  Serial.begin(9600);
  gyro_sensor.begin();
  buzzer.setpin(45);
  led.setpin(44);
  lights[0] = 32;
  lights[1] = 32;
  lights[2] = 32;
  lights[3] = 32;
  lights[4] = 32;

}

uint8_t run_command(String commandVerb, int argumentCount, String arguments[]) {
  switch(argumentCount){
    case 0:
      if (commandVerb.equals("read_light")){
         Serial.println(light_sensor.read());
      } else if (commandVerb.equals("read_sound")){
          Serial.println(sound_sensor.strength());
      } else if (commandVerb.equals("read_gyro_x")){
          Serial.println(gyro_sensor.getAngleX());
      } else if(commandVerb.equals("read_gyro_y")){
          Serial.println(gyro_sensor.getAngleY());
      } else if(commandVerb.equals("read_gyro_z")){
          Serial.println(gyro_sensor.getAngleZ());
      } else if(commandVerb.equals("read_temperature")){
          Serial.println(temperature_sensor.temperature());
      } else if(commandVerb.equals("read_sonar")){
          Serial.println(sonar_sensor.distanceCm());
      } else if(commandVerb.equals("read_line")){
//          buzzer.tone(1000, 100);
          return line_sensor.readSensors();
          Serial.println(line_sensor.readSensors());
      } else if(commandVerb.equals("stop")){
          right_motor.setMotorPwm(0);
          left_motor.setMotorPwm(0);
      } else if(commandVerb.equals("forward")){
          right_motor.setMotorPwm(-200);
          left_motor.setMotorPwm(200);
      } else if(commandVerb.equals("left")){
          right_motor.setMotorPwm(-200);
          left_motor.setMotorPwm(-200);
      } else if(commandVerb.equals("right")){
          right_motor.setMotorPwm(200);
          left_motor.setMotorPwm(200);
      } else if(commandVerb.equals("backward")){
          right_motor.setMotorPwm(200);
          left_motor.setMotorPwm(-200);
      } else if(commandVerb.equals("light_off")){
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
        Serial.println(arguments[0]);
        Serial.println(arguments[1]);
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
   return 0;
}

void stopMoving() {
   right_motor.setMotorPwm(0);
   left_motor.setMotorPwm(0);
}

void startMoving(int left, int right) {
    right_motor.setMotorPwm(-right);
    left_motor.setMotorPwm(left);
}

void setLights(int r, int g, int b) {
  led.setColor(0, r, g, b);
  led.show();
}

void turnOffLights() {
  setLights(0,0,0);
}

void playSound() {
  buzzer.tone(1000, 100);
}

int i = 0;
int isLightOn = 0;
int lightCount = 0;
int soundThreshold = 240;
void loop()
{
//  gyro_sensor.update();
  int soundReading = sound_sensor.strength();
  Serial.println(soundReading);
  lights[i] = soundReading;

  // if lights are on initially
  if (lights[i-5] < soundThreshold) {
    Serial.println("LOOP");
    int lightsOff = 0;
    for (int f = i - 5; f <= i; f++) {
      if (lights[f] >= soundThreshold) {
        // if lights go off
        lightsOff = 1;
        Serial.println("lights off");
      }
      
      if (lights[f] < soundThreshold && lightsOff == 1) {
        // if lights go back on.
        // execute command..
        Serial.println("command");
//        playSound();
        setLights(255, 255, 0);
//        startMoving(200, 200);
        isLightOn = 1;
        lightCount = 0;
        lights[f] = 120;
        lights[f-1] = 120;
        lights[f-2] = 120;
        lights[f-3] = 120;
        lights[f-4] = 120;
        lights[f-5] = 120;
      }
    }
  }

  if (isLightOn == 1) {
    if (lightCount > 20) {
      isLightOn = 0;
      lightCount = 0;
      turnOffLights();
//      stopMoving();
    }
    lightCount += 1;
  }
  if (i == 980) {
    i = 0;
  } else {
    i = i + 1;
  }
  delay(100);
}

void lightCommands() {
  int lightReading = light_sensor.read();
  Serial.println(lightReading);
  lights[i] = lightReading;

  // if lights are on initially
  Serial.println(i);
  if (lights[i-5] > 5) {
    Serial.println("LOOP");
    int lightsOff = 0;
    for (int f = i - 5; f <= i; f++) {
      if (lights[f] < 5) {
        // if lights go off
        lightsOff = 1;
      }
    
      if (lights[f] > 5 && lightsOff == 1) {
        // if lights go back on.
        // execute command..
        playSound();
        lights[f] = 32;
        lights[f-1] = 32;
        lights[f-2] = 32;
        lights[f-3] = 32;
        lights[f-4] = 32;
        lights[f-5] = 32;
      }
    }
  }
  if (i == 980) {
    i = 0;
  } else {
    i = i + 1;
  }
  delay(200);
}

void lightReacting() {
  int lightReading = light_sensor.read();
  Serial.println(lightReading);
  if (lightReading < 5 || lightReading > 600) {
    // light up
    // r, g , b
    setLights(0, 255, 255);
  } else {
    // turn off lights
    turnOffLights();
  }
  delay(200);
}

void lineFollowing() {
  int lineReading = line_sensor.readSensors();
  if (lineReading == 0) {
    startMoving(120, 120);
  } else if (lineReading == 1) {
    startMoving(60, 120);
  } else if (lineReading == 2) {
    startMoving(120, 60);
  } else {
    stopMoving();
  }
  delay(200);
}
