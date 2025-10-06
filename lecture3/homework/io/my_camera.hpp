#ifndef MY_CAMERA_HPP
#define MY_CAMERA_HPP

#include "hikrobot/include/MvCameraControl.h"
#include <opencv2/opencv.hpp>

class myCamera {
public:
    myCamera();
    ~myCamera();
    cv::Mat read();

private:
    void* handle_;
    MV_FRAME_OUT raw_;
    bool initializeCamera_();
    bool setCameraParameters_();
    void cleanup_();
};

#endif // MY_CAMERA_HPP