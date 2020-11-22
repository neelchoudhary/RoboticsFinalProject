
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <math.h>
#include "viz.hh"

#include "ranger-library.hh"

using namespace std;


// // Add an angle in the clockwise direction.
// double angleAdd(double currAngle, double additionAngle) {
// 	if (newAngle > 180) {
// 		newAngle = newAngle - 360;
// 	}
// 	if (currAngle <= -180) {
// 		currAngle = currAngle + 360;
// 	}
// 	double newAngle = (currAngle + additionAngle) % 360;
// 	return newAngle;
// }

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

int main(int argc, char* argv[]) {
	// int status =  viz_run(argc, argv);
	// cout << status << endl;
	create_robot_connection("/dev/ttyUSB0");
	sleep(2);

	// lineReadings
	double lineReadings[1000];
	lineReadings[0] = 0;
	lineReadings[1] = 0;
	lineReadings[2] = 0;
	lineReadings[3] = 0;
	lineReadings[4] = 0;

	// spinning stuff
	double startSpinZ = 0;
	int spinTime = 0;

	// valid paths
	bool leftPath = false;
	bool rightPath = false;
	bool forwardPath = false;

	int turnTime = 0;

	int i = 5;
	// 0 = forward, 1 = right, 2 = left, 3 = prepare for intersection, 4 = start spin, 5 = stop, 6 = TURN right, 7 = TURN left
	int turnDir = 0;
	while (true) {
		std::array<double, 5> sensorReadings = read_all();
		double lineReading = sensorReadings[0];
		double rightLineReading = sensorReadings[1];
		double z_pos = sensorReadings[4];
		// cout << "Line Sensor: " + to_string(lineReading) << endl;
		// cout << "Right sensor: " + to_string(rightLineReading) << endl;
		// cout << "z sensor: " + to_string(z_pos) << endl;

		int moveSpeed = 120;
		int turnSpeed = 150;
		lineReadings[i] = lineReading;
		
		if (turnDir == 0) {
			if (lineReading == 0 && rightLineReading != 0) {
				// cout << "Move!" << endl;
				tank_drive(moveSpeed, moveSpeed);
			} else if (rightLineReading == 0) {
				// turn right at intersection
				// actually we spinny
				turnDir = 3;
				spinTime = 0;
				startSpinZ = z_pos;
				// cout << "Intersection!" << endl;
				// turnDir = 2;
				// cout << "Turn right" << endl;
			} else if (lineReading == 1) {
				// turn right
				turnDir = 1;
				// tank_drive(moveSpeed, 0);
			} else if (lineReading == 2) {
				// turn left
				turnDir = 2;
				// tank_drive(0, moveSpeed);
			} else {
				if (i > 5) {
					if (lineReadings[i-1] == 1 || lineReadings[i-2] == 1 || lineReadings[i-3] == 1 || lineReadings[i-4] == 1 || lineReadings[i-5] == 1) {
						// tank_drive(moveSpeed, 0);
						turnDir = 1;
					} else if (lineReadings[i-1] == 2 || lineReadings[i-2] == 2 || lineReadings[i-3] == 2 || lineReadings[i-4] == 2 || lineReadings[i-5] == 2) {
						// tank_drive(0, moveSpeed);
						turnDir = 2;
					} else {
						stop();
					}
				} else {
					stop();
				}
			}
		} else if (turnDir == 1) {
			tank_drive(turnSpeed, 0);
			if (lineReading == 0) {
				turnDir = 0;
			}
		} else if (turnDir == 2) {
			tank_drive(0, turnSpeed);
			if (lineReading == 0) {
				turnDir = 0;
			}
		} else if (turnDir == 3) {
			tank_drive(80, 80);
			if (rightLineReading == 3) {
				turnDir = 4;
			}
		} else if (turnDir == 4) {
			tank_drive(-turnSpeed, turnSpeed);
			spinTime += 1;
			if (abs(angleDiff(startSpinZ, z_pos)) < 15 && spinTime >= 15) {
				turnDir = 5;
			}
			if (abs(angleDiff(startSpinZ + 90, z_pos)) < 15 && lineReading == 0) {
				// there is a path to the right
				cout << "RIGHT " << to_string(abs(angleDiff(startSpinZ + 90, z_pos))) << endl;
				rightPath = true;
			}
			if (abs(angleDiff(startSpinZ, z_pos)) < 15 && lineReading == 0) {
				// there is a path continueing forward
				cout << "FORWARD " << to_string(abs(angleDiff(startSpinZ, z_pos))) << endl;
				forwardPath = true;
			}
			if (abs(angleDiff(startSpinZ + 270, z_pos)) < 15 && lineReading == 0) {
				// there is a path to the left
				cout << "LEFT " << to_string(abs(angleDiff(startSpinZ + 270, z_pos))) << endl;
				leftPath = true;
			}
		} else if (turnDir == 5) {
			tank_drive(0,0);
			if (rightPath) {
				turnDir = 7;
				turnTime = 0;
				bool leftPath = false;
				bool rightPath = false;
				bool forwardPath = false;				
			} else if (leftPath) {
				turnDir = 6;
				turnTime = 0;
				bool leftPath = false;
				bool rightPath = false;
				bool forwardPath = false;				
			} else if (forwardPath) {
				turnDir = 0;
				turnTime = 0;
				bool leftPath = false;
				bool rightPath = false;
				bool forwardPath = false;				
			}
		} else if (turnDir == 6) {
			tank_drive(turnSpeed, -turnSpeed);
			if (turnTime > 15 && lineReading == 0) {
				// there is a path to the left
				turnDir = 0;
			}
			turnTime += 1;
		} else if (turnDir == 7) {
			tank_drive(-turnSpeed, turnSpeed);
			if (turnTime > 15 && lineReading == 0) {
				// there is a path to the right
				turnDir = 0;
			}
			turnTime += 1;
		}

		if (i >= 980) {
			i = 5;
		} else {
			i += 1;
		}
		sleep(0.05);
	}
}

