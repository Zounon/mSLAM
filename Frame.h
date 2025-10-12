#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <sophus/se3.hpp>
#include <vector>
#include <iostream>
#include <mutex>

class Frame
{
public:
    bool is_valid = false;
    cv::Mat img;
    cv::Mat bw;
    std::vector<cv::Point2f> keypts;
    std::mutex mutex;
	Sophus::SE3<double> pose = Sophus::SE3<double>();

    Frame(cv::VideoCapture& cap);

    bool isKeyFrame() {
        return is_keyframe_;
    }

    void setKeyFrame() {
        is_keyframe_ = true;
    }


private:
    bool is_keyframe_ = false;
	Sophus::SE3<double> getPose();
    void setPose(Sophus::SE3<double> new_pose);
};