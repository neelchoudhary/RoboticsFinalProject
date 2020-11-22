#ifndef ROBOT_HH
#define ROBOT_HH

#include <gazebo/gazebo_config.h>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>

class GazeboRobot {
  public:
    void (*on_update)(GazeboRobot*);
    // position
    float pos_x;
    float pos_y;
    // distance from goal
    float range;
    // positional angle (z)
    float pos_t;
    // sound measurement
    int noise;

    GazeboRobot(int argc, char* argv[], void (*cb)(GazeboRobot*));
    ~GazeboRobot();

    bool at_goal();
    void execute();
    void end();

    void set_vel(double lvel, double rvel);
    void on_scan(ConstSonarStampedPtr &msg);
    void on_sound(ConstIntPtr &msg);
    void on_pose(ConstPoseStampedPtr &msg);

  private:
    bool is_complete;

    gazebo::transport::NodePtr node;
    gazebo::transport::PublisherPtr vel_pub;
    gazebo::transport::SubscriberPtr scan_sub;
    gazebo::transport::SubscriberPtr mic_sub;
    gazebo::transport::SubscriberPtr pose_sub;
    gazebo::transport::SubscriberPtr frame_sub;
};

#endif
