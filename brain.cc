
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "viz.hh"

#include "ranger-library.hh"

using namespace std;

void line_following(double lineReading, int turnDir, double lineReadings[1000], int i) {
	int moveSpeed = 150;
	lineReadings[i] = lineReading;
	
	if (turnDir == 0) {
		if (lineReading == 0) {
			tank_drive(moveSpeed, moveSpeed);
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
		tank_drive(moveSpeed, 0);
		if (lineReading == 0) {
			turnDir = 0;
		}
	} else if (turnDir == 2) {
		tank_drive(0, moveSpeed);
		if (lineReading == 0) {
			turnDir = 0;
		}
	}
}

int
main(int argc, char* argv[])
{
	// int status =  viz_run(argc, argv);
	// cout << status << endl;
	create_robot_connection("/dev/ttyUSB0");
	sleep(2);

	double lineReadings[1000];
	lineReadings[0] = 0;
	lineReadings[1] = 0;
	lineReadings[2] = 0;
	lineReadings[3] = 0;
	lineReadings[4] = 0;



	int i = 5;
	// 0 = forward, 1 = right, 2 = left
	int turnDir = 0;
	while (true) {
		std::array<double, 5> sensorReadings = read_all();
		double lineReading = sensorReadings[0];
		cout << lineReading << endl;
		line_following(lineReading, turnDir, lineReadings, i);
		if (i >= 980) {
			i = 5;
		} else {
			i += 1;
		}
		sleep(0.05);
	}
}

