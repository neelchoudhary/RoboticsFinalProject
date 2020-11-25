#ifndef ROBOT_HH
#define ROBOT_HH

#include <gazebo/gazebo_config.h>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>

#include <opencv2/core/mat.hpp>

class Robot {
  public:
    virtual ~Robot() = 0;
    virtual std::array<double, 5> read_all() = 0;
    virtual int get_line_status() = 0;
    virtual int get_line_status2() = 0;
    virtual double get_noise_sensor() = 0;
    virtual void set_vel(double lvel, double rvel) = 0;
    virtual void do_stuff() = 0;
};

class GzRobot : public Robot {
  public:
    void (*on_update)(Robot*);

    float pos_x;
    float pos_y;
    float range;
    float pos_t;
    int noise;
    int line_status;
    int line_status2;
    cv::Mat frame;
    cv::Mat frame2;

    GzRobot(int argc, char* argv[], void (*cb)(Robot*));
    ~GzRobot();

    bool at_goal();
    void done();

    std::array<double, 5> read_all();
    int get_line_status();
    int get_line_status2();
    int get_line_statusz();
    int get_line_status2z();
    double get_noise_sensor();
    void set_vel(double lvel, double rvel);
    void do_stuff();

    void on_frame(ConstImageStampedPtr &msg);
    void on_frame2(ConstImageStampedPtr &msg);
    void on_scan(ConstSonarStampedPtr &msg);
    void on_sound(ConstIntPtr &msg);
    void on_pose(ConstPoseStampedPtr &msg);

  private:
    bool task_done;

    gazebo::transport::NodePtr node;
    gazebo::transport::PublisherPtr vel_pub;
    gazebo::transport::SubscriberPtr scan_sub;
    gazebo::transport::SubscriberPtr mic_sub;
    gazebo::transport::SubscriberPtr pose_sub;
    gazebo::transport::SubscriberPtr frame_sub;
    gazebo::transport::SubscriberPtr frame_sub2;
};

class RgRobot : public Robot {
    public:
      RgRobot(int argc, char* argb[], void (*cb)(Robot*));
      ~RgRobot();

      std::array<double, 5> read_all();
      int get_line_status();
      int get_line_status2();
      double get_noise_sensor();
      void set_vel(double lvel, double rvel);
      void do_stuff();

      void (*on_update)(Robot*);
};

#endif
