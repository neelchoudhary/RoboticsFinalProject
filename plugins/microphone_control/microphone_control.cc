
#include <iostream>
#include <algorithm>

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <math.h>

using std::string;
using std::vector;
using namespace gazebo;
using physics::ModelPtr;
using physics::JointControllerPtr;
using ignition::math::Pose3d;
using common::Time;

const int NUM_SOURCES = 2;
int SOURCES[][NUM_SOURCES] = {{3, 3}, {-3, -3}};
int SOURCES_DECIBELS[NUM_SOURCES] = {60, 60};

class MicrophoneControlPlugin : public ModelPlugin
{
public:
    physics::ModelPtr model;

    transport::NodePtr node;

    transport::SubscriberPtr vel_sub;
    transport::SubscriberPtr stat_sub;

    transport::PublisherPtr  mic_pub;

    MicrophoneControlPlugin() {}

    virtual void
    Load(physics::ModelPtr model, sdf::ElementPtr sdf)
    {
        this->model = model;

        auto model_name = this->model->GetName();
        auto world_name = this->model->GetWorld()->Name();

        std::cerr << "hello from MicControlPlugin" << std::endl;
        std::cerr << "world: " << world_name << std::endl;
        std::cerr << "model: " << model_name << std::endl;

        this->node = transport::NodePtr(new transport::Node());
        this->node->Init(world_name);

        string stats_topic = "~/world_stats";
        this->stat_sub = this->node->Subscribe(
            stats_topic, &MicrophoneControlPlugin::OnStats, this);
        std::cerr << "Subscribed world_stats: "
                  << this->stat_sub->GetTopic() << std::endl;

        string mic_topic = "~/" + model_name + "/mic";
        this->mic_pub = this->node->Advertise<gazebo::msgs::Int>(mic_topic);
        std::cout << "Advertised microphone" << std::endl;

        std::cout << "microphone control loaded" << std::endl;
    }

    double distanceCalculate(double x1, double y1, double x2, double y2)
    {
    	double x = fabs(x1 - x2);
    	double y = fabs(y1 - y2);
    	double dist;

    	dist = pow(x, 2) + pow(y, 2);  //calculating Euclidean distance
    	dist = sqrt(dist);

    	return dist;
    }

    double
    clamp(double xmin, double xx, double xmax)
    {
        if (xx < xmin) return xmin;
        return xx;
    }


    int
    make_mic_msg(int x, int y)
    {
        int loudest = 0;
        for (int i = 0; i < NUM_SOURCES; i++) {
            int source_x = SOURCES[i][0];
            int source_y = SOURCES[i][1];
            int source_decibels = SOURCES_DECIBELS[i];

            double dist = distanceCalculate(x, y, source_x, source_y);

            // decibel intensity degrade by the inverse square law
            double distance = pow(dist, 2);
            double denom = clamp(1.0, dist, 1000);
            double intensity = 1 / denom;
            double decibels_float = source_decibels * intensity;

            int decibels = round(decibels_float);

            if (decibels > loudest)
            {
                loudest = decibels;
            }
        }

        return loudest;
    }

    void
    OnStats(ConstAnyPtr &_msg)
    {
        auto pose = this->model->WorldPose();
        auto pos = pose.Pos();
        double intensity = make_mic_msg(pos.X(), pos.Y());
        gazebo::msgs::Int msg;
        msg.set_data(intensity);
        this->mic_pub->Publish(msg);
    }
};

GZ_REGISTER_MODEL_PLUGIN(MicrophoneControlPlugin)
