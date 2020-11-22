#include <iostream>
#include <string>
#include <vector>
#include "robot.hh"

using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace gazebo;
using namespace gazebo::transport;

const double GOAL_X = 20.0;
const double GOAL_Y = 0.0;

Robot::Robot(int argc, char* argv[], void (*cb)(Robot*))
    : on_update(cb), is_complete(false) {
    client::setup(argc, argv);
    node = NodePtr(new Node());
    node->Init();

    vel_pub = node->Advertise<gazebo::msgs::Any>("~/tankbot0/vel_cmd");
    vel_pub->WaitForConnection();

    cout << "Here" << endl;
    scan_sub = node->Subscribe(
        string("~/tankbot0/tankbot/ultrasonic_sensor/link/sonar/sonar"),
        &Robot::on_scan,
        this,
        false
    );

    mic_sub = node->Subscribe(
        string("~/tankbot0/mic"),
        &Robot::on_sound,
        this,
        false
    );

    pose_sub = node->Subscribe(
        string("~/tankbot0/pose"),
        &Robot::on_pose,
        this,
        false
    );

    cout << "robot created" << endl;
}

Robot::~Robot() {
    client::shutdown();
    cout << "robot destroyed" << endl;
}

void Robot::do_stuff() {
    while (!is_complete) {
        gazebo::common::Time::MSleep(10);

        if (this->at_goal()) {
            this->set_vel(0.0, 0.0);
            this->end();
        }
    }

    gazebo::common::Time::MSleep(100);
}

bool Robot::at_goal() {
    // measured by the range difference from sonar
    // double dx = GOAL_X - this->pos_x;
    // double dy = GOAL_Y - this->pos_y;
    // return (abs(dx) < 0.75 && abs(dy) < 0.75);
}

/* end program */
void Robot::end() {
    this->is_complete = true;
    cout << "reached goal" << endl;
}

void Robot::set_vel(double lvel, double rvel) {
    //cout << "set_vel: " << lvel << "," << rvel << endl;
    int xx = 128 + int(lvel * 25.0);
    int yy = 128 + int(rvel * 25.0);
    auto msg = msgs::ConvertAny(xx * 256 + yy);
    //cout << "send vmsg = " << msg.int_value() << endl;
    this->vel_pub->Publish(msg);
}

void Robot::on_scan(ConstSonarStampedPtr &msg) {
    //cout << "Receiving the Sonar message from Sonar Sensor" << endl;
    msgs::Sonar sonar = msg->sonar();
    double hit_range = sonar.range();
    range = hit_range;

    this->on_update(this);
}

void Robot::on_sound(ConstIntPtr &msg) {

    this->noise = msg->data();
    this->on_update(this);
}

void Robot::on_pose(ConstPoseStampedPtr &msg) {
    auto pos = msg->pose().position();
    this->pos_x = pos.x();
    this->pos_y = pos.y();

    auto rot = msg->pose().orientation();
    ignition::math::Quaternion<double> qrot(rot.w(), rot.x(), rot.y(), rot.z());
    this->pos_t = qrot.Yaw();

    this->on_update(this);
}