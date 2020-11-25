#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/mat.hpp>

#include "robot.hh"

using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace gazebo;
using namespace gazebo::transport;

const double GOAL_X = 20.0;
const double GOAL_Y = 0.0;

double
clamp(double xmin, double xx, double xmax)
{
    if (xx < xmin) return xmin;
    if (xx > xmax) return xmax;
    return xx;
}

GzRobot::GzRobot(int argc, char* argv[], void (*cb)(Robot*))
    : on_update(cb), task_done(false)
{
    client::setup(argc, argv);
    node = NodePtr(new Node());
    node->Init();

    vel_pub = node->Advertise<gazebo::msgs::Any>("~/tankbot0/vel_cmd");
    vel_pub->WaitForConnection();

    cout << "Here" << endl;
    scan_sub = node->Subscribe(
        string("~/tankbot0/tankbot/ultrasonic_sensor/link/sonar/sonar"),
        &GzRobot::on_scan,
        this,
        false
    );

    mic_sub = node->Subscribe(
        string("~/tankbot0/mic"),
        &GzRobot::on_sound,
        this,
        false
    );

    pose_sub = node->Subscribe(
        string("~/tankbot0/pose"),
        &GzRobot::on_pose,
        this,
        false
    );

    frame_sub = node->Subscribe(
        string("~/tankbot0/tankbot/camera_sensor/link/camera/image"),
        &GzRobot::on_frame,
        this,
        false
    );

    frame_sub2 = node->Subscribe(
        string("~/tankbot0/tankbot/camera_sensor/link_2/camera/image"),
        &GzRobot::on_frame2,
        this,
        false
    );

    cout << "robot created" << endl;
}

GzRobot::~GzRobot()
{
    client::shutdown();
    cout << "robot destroyed" << endl;
}

void
GzRobot::do_stuff()
{
    while (!task_done) {
        gazebo::common::Time::MSleep(10);

        if (this->at_goal()) {
            cout << "WE AT GOAL" << endl;
            this->set_vel(0.0, 0.0);
            this->done();
        }
    }

    gazebo::common::Time::MSleep(100);
}

bool
GzRobot::at_goal()
{
    double dx = GOAL_X - this->pos_x;
    double dy = GOAL_Y - this->pos_y;
    return (abs(dx) < 0.75 && abs(dy) < 0.75);
}

void
GzRobot::done()
{
    this->task_done = true;
}

void
GzRobot::set_vel(double lvel, double rvel)
{
    // auto r_error = lvel * ((rand() % 21) - 10) * 0.01;
    // auto l_error = rvel * ((rand() % 21) - 10) * 0.01;

    // lvel = clamp(-4, lvel + l_error, 4);
    // rvel = clamp(-4, rvel + r_error, 4);

    // cout << "set_vel: " << lvel << "," << rvel << endl;
    // int xx = 128 + int(lvel * 25.0);
    // int yy = 128 + int(rvel * 25.0);
    // auto msg = msgs::ConvertAny(xx * 256 + yy);
    // cout << "send vmsg = " << msg.int_value() << endl;
    // this->vel_pub->Publish(msg);
    lvel = clamp(-10, lvel, 10);
    rvel = clamp(-10, rvel, 10);
    // cout << "set_vel: " << lvel << "," << rvel << endl;
    int xx = 128 + int(lvel * 25.0);
    int yy = 128 + int(rvel * 25.0);
    auto msg = msgs::ConvertAny(xx * 256 + yy);
    // cout << "send vmsg = " << msg.int_value() << endl;
    this->vel_pub->Publish(msg);
}

void
GzRobot::on_frame(ConstImageStampedPtr &msg)
{
    msgs::Image image = msg->image();

    char* data = (char*)malloc(image.data().size());
    memcpy(data, image.data().c_str(), image.data().size());
    cv::Mat temp(image.height(), image.width(), CV_8UC3, data);
    cv::Mat temp2 = temp.clone();
    cv::cvtColor(temp, temp2, cv::COLOR_RGBA2BGRA);
    this->frame = temp2.clone();
    assert(this->frame.size().height > 0);
    free(data);

    // each frame, update the line status
    this->line_status = this->get_line_status();
    this->on_update(this);
}

void
GzRobot::on_frame2(ConstImageStampedPtr &msg)
{
    msgs::Image image = msg->image();

    char* data = (char*)malloc(image.data().size());
    memcpy(data, image.data().c_str(), image.data().size());
    cv::Mat temp(image.height(), image.width(), CV_8UC3, data);
    cv::Mat temp2 = temp.clone();
    cv::cvtColor(temp, temp2, cv::COLOR_RGBA2BGRA);
    this->frame2 = temp2.clone();
    assert(this->frame2.size().height > 0);
    free(data);

    // each frame, update the line status
    this->line_status2 = this->get_line_status2();
    this->on_update(this);
}

int
GzRobot::get_line_status() {
    if (!this->frame.empty()) {
        // there is a 255 separator after every 3 rgb values, so we get 4
        cv::Vec4b left_front = this->frame.at<cv::Vec4b>(0, 0);
        cv::Vec4b left_back = this->frame.at<cv::Vec4b>(1, 0);
        cv::Vec4b right_front = this->frame.at<cv::Vec4b>(0, 1);
        cv::Vec4b right_back = this->frame.at<cv::Vec4b>(1, 1);

        int leftfront = false;
        int leftback = false;
        int rightfront = false;
        int rightback = false;

        // find dark lines
        if (left_front[0] < 100 && left_front[1] < 100 && left_front[2] < 100)
        {
            leftfront = true;
        }
        if (left_back[0] < 100 && left_back[1] < 100 && left_back[2] < 100)
        {
            leftback = true;
        }
        if (right_front[0] < 100 && right_front[1] < 100 && right_front[2] < 100)
        {
            rightfront = true;
        }
        if (right_back[0] < 100 && right_back[1] < 100 && right_back[2] < 100)
        {
            rightback = true;
        }

        // return status like the mBot ranger
        if (leftfront && leftback && rightfront && rightback)
        {
            return 3;
        }
        else if (leftfront && leftback)
        {
            return 1;
        }
        else if (rightfront && rightback)
        {
            return 2;
        }

    }
    return 0;
}

int
GzRobot::get_line_status2() {
    if (!this->frame2.empty()) {
        // there is a 255 separator after every 3 rgb values, so we get 4
        cv::Vec4b left_front = this->frame2.at<cv::Vec4b>(0, 0);
        cv::Vec4b left_back = this->frame2.at<cv::Vec4b>(1, 0);
        cv::Vec4b right_front = this->frame2.at<cv::Vec4b>(0, 1);
        cv::Vec4b right_back = this->frame2.at<cv::Vec4b>(1, 1);

        int leftfront = false;
        int leftback = false;
        int rightfront = false;
        int rightback = false;
        cout << "DOES THIS HAPPEN" << endl;

        // find dark lines
        if (left_front[0] < 100 && left_front[1] < 100 && left_front[2] < 100)
        {
            leftfront = true;
        }
        if (left_back[0] < 100 && left_back[1] < 100 && left_back[2] < 100)
        {
            leftback = true;
        }
        if (right_front[0] < 100 && right_front[1] < 100 && right_front[2] < 100)
        {
            rightfront = true;
        }
        if (right_back[0] < 100 && right_back[1] < 100 && right_back[2] < 100)
        {
            rightback = true;
        }

        // return status like the mBot ranger
        if (leftfront && leftback && rightfront && rightback)
        {
            return 3;
        }
        else if (leftfront && leftback)
        {
            return 1;
        }
        else if (rightfront && rightback)
        {
            return 2;
        }

    }
    cout << "FUCK" << endl;
    return 0;
}


std::array<double, 5> 
GzRobot::read_all() {
    double lineStatus1 = (double)this->get_line_status();
    double lineStatus2 = (double)this->get_line_status2();
    double sonar = this->range;
    double pose_z = this->pos_t;
    std::array<double, 5> ar1{{lineStatus1, lineStatus2, sonar, pose_z, -1}}; 
    return ar1;
}

void
GzRobot::on_scan(ConstSonarStampedPtr &msg)
{
    //cout << "Receiving the Sonar message from Sonar Sensor" << endl;
    msgs::Sonar sonar = msg->sonar();
    double hit_range = sonar.range();
    range = hit_range;

    this->on_update(this);
}

void
GzRobot::on_sound(ConstIntPtr &msg)
{

    this->noise = msg->data();
    this->on_update(this);
}

double
GzRobot::get_noise_sensor() {
  return this->noise;
}

void
GzRobot::on_pose(ConstPoseStampedPtr &msg)
{
    auto x_error = ((rand() % 21) - 10) * 0.02;
    auto y_error = ((rand() % 21) - 10) * 0.02;

    auto pos = msg->pose().position();
    // no x and y error
    this->pos_x = pos.x();
    this->pos_y = pos.y();

    auto rot = msg->pose().orientation();
    ignition::math::Quaternion<double> qrot(rot.w(), rot.x(), rot.y(), rot.z());
    this->pos_t = qrot.Yaw();

    this->on_update(this);
}

Robot::~Robot() {}
