#include "MeAuriga.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <math.h>

MeLightSensor light_sensor(2);
MeSoundSensor sound_sensor(5);
MeGyro gyro_sensor(1, 0x69);
MeTemperature temperature_sensor(PORT_13);
MeUltrasonicSensor sonar_sensor(9);
MeLineFollower line_sensor(10);
MeLineFollower line_sensor_right(8);
MeEncoderOnBoard right_motor(SLOT1);
MeEncoderOnBoard left_motor(SLOT2);
MeBuzzer buzzer;
MeRGBLed led;


char commandArray[64];
char cmd_idx = 0;


void setup()
{
  Serial.begin(9600);
  gyro_sensor.begin();
  buzzer.setpin(45);
  led.setpin(44);
  turnOffLights();
}

int currLeftMotorSpeed = -1;
int currRightMotorSpeed = -1;

void stopMoving() {
   if (currLeftMotorSpeed != 0 || currRightMotorSpeed != 0) {
       currLeftMotorSpeed = 0;
       currRightMotorSpeed = 0;
       right_motor.setMotorPwm(0);
       left_motor.setMotorPwm(0);
   }
}

void startMoving(int left, int right) {
    if (currLeftMotorSpeed != left || currRightMotorSpeed != right) {
       currLeftMotorSpeed = left;
       currRightMotorSpeed = right;
       right_motor.setMotorPwm(-right);
       left_motor.setMotorPwm(left);
    }

}

void setLights(int x, int r, int g, int b) {
  led.setColor(0,0,0,0);
  led.setColor(x, r, g, b);
  led.show();
}

void turnOffLights() {
  setLights(0,0,0,0);
}

void playSound() {
  buzzer.tone(1000, 100);
}

double angleDiff(double angle1, double angle2) {
	if (angle1 < 0) {
		angle1 += 360;
	}
	if (angle2 < 0) {
		angle2 += 360;
	}

    double diff = fmod(angle1 - angle2, 360);

    if (diff < -180) {
        diff += 360;
    }

    if (diff > 180) {
        diff -= 360;
    }

    return diff;
}

// lineReadings
double lineReadings[100];
//lineReadings[0] = 0;
//lineReadings[1] = 0;
//lineReadings[2] = 0;
//lineReadings[3] = 0;
//lineReadings[4] = 0;

// sonarReadings during spin
double sonarReadings[500];
double angleReadings[500];
int sonar_i = 0;

// spinning stuff
double startSpinZ = 0;
int spinTime = 0;

// valid paths
bool leftPath = false;
bool rightPath = false;
bool forwardPath = false;

int turnTime = 0;

int i = 5;
// 0 = forward, 1 = left, 2 = right, 3 = prepare for intersection, 4 = start spin, 5 = stop, 6 = TURN left, 7 = TURN right, 8 = GO straight, 9 = AT GOAL.
int turnDir = 0;

void loop()
{
  gyro_sensor.update();
  double lineReading = line_sensor.readSensors();
	double rightLineReading = line_sensor_right.readSensors();
	double sonarReading = sonar_sensor.distanceCm();
	double z_pos = gyro_sensor.getAngleZ(); 

	int moveSpeed = 120;
	int turnSpeed = 150;
	lineReadings[i] = lineReading;
	
	if (turnDir == 0) {
    setLights(1,255,0,0);
		if (sonarReading < 7) {
      Serial.println("WE FOUND THE GOAL");
			stopMoving();
			turnDir = 9;
		}
		if (lineReading == 0 && rightLineReading != 0) {
			// cout << "Move!" << endl;
			startMoving(moveSpeed, moveSpeed);
		} else if (rightLineReading == 0) {
			// we are at intersection
      Serial.println("INTERSECTION");
			turnDir = 3;
			spinTime = 0;
			startSpinZ = z_pos;
		} else if (lineReading == 1) {
			// turn left
			turnDir = 1;
		} else if (lineReading == 2) {
			// turn right
			turnDir = 2;
		} else {
			if (i > 5) {
				if (lineReadings[i-1] == 1 || lineReadings[i-2] == 1 || lineReadings[i-3] == 1 || lineReadings[i-4] == 1 || lineReadings[i-5] == 1) {
					turnDir = 2;
				} else if (lineReadings[i-1] == 2 || lineReadings[i-2] == 2 || lineReadings[i-3] == 2 || lineReadings[i-4] == 2 || lineReadings[i-5] == 2) {
					turnDir = 1;
				} else {
					stopMoving();
				}
			} else {
				stopMoving();
			}
		}
	} else if (turnDir == 1) {
		// turning left
    setLights(2,255,0,0);
		startMoving(0, turnSpeed);
		if (lineReading == 0) {
			turnDir = 0;
		}
	} else if (turnDir == 2) {
		// turning right
    setLights(2,0,0,255);
		startMoving(turnSpeed, 0);
		if (lineReading == 0) {
			turnDir = 0;
		}
	} else if (turnDir == 3) {
		// at intersection, preparing for spin.
		setLights(3,255,255,255);
		startMoving(80, 80);
		if (rightLineReading == 3) {
      Serial.println("Starting spin...");
			turnDir = 4;
		}
	} else if (turnDir == 4) {
		// spinning
    setLights(4,255,255,255);
		startMoving(-turnSpeed, turnSpeed);
		spinTime += 1;
//    Serial.println(abs(angleDiff(startSpinZ, z_pos)));
		if (abs(angleDiff(startSpinZ, z_pos)) < 15 && spinTime >= 15) {
      Serial.println("Done spinning, evaluating options..");
			turnDir = 5;
		}
		if (abs(angleDiff(startSpinZ + 90, z_pos)) < 15 && lineReading == 0) {
			// there is a path to the right
			// cout << "RIGHT " << to_string(abs(angleDiff(startSpinZ + 90, z_pos))) << endl;
			rightPath = true;
		}
		if (abs(angleDiff(startSpinZ, z_pos)) < 15 && lineReading == 0) {
			// there is a path continuing forward
			// cout << "FORWARD " << to_string(abs(angleDiff(startSpinZ, z_pos))) << endl;
			forwardPath = true;
		}
		if (abs(angleDiff(startSpinZ + 270, z_pos)) < 15 && lineReading == 0) {
			// there is a path to the left
			// cout << "LEFT " << to_string(abs(angleDiff(startSpinZ + 270, z_pos))) << endl;
			leftPath = true;
		}

		sonarReadings[sonar_i] = sonarReading;
		angleReadings[sonar_i] = z_pos;
		sonar_i += 1;
	} else if (turnDir == 5) {
    setLights(5,255,255,255);
		stopMoving();
		double sonarDiffs[sonar_i-1];
		double maxDiff = 0;
		int maxDiffIndex = 0;
		// calculate diffs and get largest diff
		for (int ii = 0; ii < sonar_i-1; ii++) {
			sonarDiffs[ii] = abs(sonarReadings[ii] - sonarReadings[ii+1]);
			if (sonarDiffs[ii] > maxDiff) {
				maxDiff = sonarDiffs[ii];
				maxDiffIndex = ii;
			}
		}
		sonarDiffs[maxDiffIndex] = 0;
		double secondMaxDiff = 0;
		int secondMaxDiffIndex = 0;
		// get second largest diff
		for (int ii = 0; ii < sonar_i-1; ii++) {
			if (sonarDiffs[ii] > secondMaxDiff) {
				secondMaxDiff = sonarDiffs[ii];
				secondMaxDiffIndex = ii;
			}
		}
		double goal_angle = 0;
		if (maxDiffIndex < secondMaxDiffIndex) {
			// if first max comes before second max.
			goal_angle = angleReadings[maxDiffIndex+1];
		} else {
			// if second max comes before first max.
			goal_angle = angleReadings[secondMaxDiffIndex+1];
		}
		// example
		// 21, 23, 24, 50, 51, 50, 28,
		// 02, 01, 26, 01, 01, 22.
		//  0   1   2   3   4   5   6 


		// calculate angle between goal and each available turn.
		
		double rightDiff = abs(angleDiff(goal_angle, startSpinZ + 90));
		double leftDiff = abs(angleDiff(goal_angle, startSpinZ + 270));
		double forwardDiff = abs(angleDiff(goal_angle, startSpinZ));

		if (!rightPath) {
			rightDiff = 1000000;
		}
		if (!leftPath) {
			leftDiff = 1000000;
		}
		if (!forwardPath) {
			forwardDiff = 1000000;
		}

		if (rightDiff < leftDiff && rightDiff < forwardDiff) {
     Serial.println("Turning right");
			// turn right
			turnDir = 7;
			turnTime = 0;
			bool leftPath = false;
			bool rightPath = false;
			bool forwardPath = false;
		} else if (leftDiff < rightDiff && leftDiff < forwardDiff) {
      Serial.println("Turning left");
			// turn left
			turnDir = 6;
			turnTime = 0;
			bool leftPath = false;
			bool rightPath = false;
			bool forwardPath = false;	
		} else if (forwardDiff < rightDiff && forwardDiff < leftDiff) {
      Serial.println("Heading straight");
			// go straight
			turnDir = 0;
			turnTime = 0;
			bool leftPath = false;
			bool rightPath = false;
			bool forwardPath = false;
		}
		sonar_i = 0;
	} else if (turnDir == 6) {
    setLights(6,255,0,0);
		startMoving(-turnSpeed, turnSpeed);
		if (turnTime > 15 && lineReading == 0) {
      Serial.println("Done turning left, Going straight again");
			// there is a path to the left
			turnDir = 0;
		}
		turnTime += 1;
	} else if (turnDir == 7) {
    setLights(6,0,255,0);
		startMoving(turnSpeed, -turnSpeed);
		if (turnTime > 15 && lineReading == 0) {
      Serial.println("Done turning right, Going straight again");
			// there is a path to the right
			turnDir = 0;
		}
		turnTime += 1;
	} else if (turnDir == 8) {
    setLights(6,0,0,255);
		startMoving(turnSpeed, turnSpeed);
		if (turnTime > 15) {
      Serial.println("Past intersection, continuing to go straight");
			turnDir = 15;
		}
		turnTime += 1;
	} else if (turnDir == 9) {
    setLights(0,0,255,0);
		stopMoving();
		// do something celebratory??
		// BREAK();
	}

	if (i >= 95) {
		i = 5;
	} else {
		i += 1;
	}
  delay(50);
}
