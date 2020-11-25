
#include <iostream>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "cam.hh"

using namespace cv;
using namespace std;

void
cam_init()
{
    namedWindow("camera", 1);
    cout << "named a window" << endl;
}

void
cam_show(Mat frame)
{
    cout << "cam_show" << endl;
    if (frame.size().width > 0) {
        imshow("camera", frame);
        cv::waitKey(1);
        cout << "showed a frame" << endl;
    }
}
